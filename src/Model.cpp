#include "Model.hpp"

swSim::Model::Model() {
    modMaterialsList.clear(); 
    modExcitationsList.clear(); 

    parseNames.clear();
    parseNames.push_back("time");
    parseNames.push_back("geometry");
    parseNames.push_back("materials");
    parseNames.push_back("excitations");
    modS.numTimeSteps = 0;
    modS.dt = 0.0;
    modS.numMaterials = 0;
    modS.numExcitations = 0; 
}

swSim::Model::~Model() {
}

void swSim::Model::Init(uint32_t TimeSteps, double dt, uint8_t numMat, uint8_t numExc)  {
    //assign relevant transducer parameters
    modS.numTimeSteps = TimeSteps;
    modS.dt = dt;
    modS.numMaterials = numMat;
    modS.numExcitations = numExc;

    modMaterialsList.resize(modS.numMaterials);
    modExcitationsList.resize(modS.numExcitations);
    return; 
}
void swSim::Model::Init(uint32_t TimeSteps, double dt, uint8_t numMat, uint8_t numExc, const char *inFname, const char *inPname)  {
    //assign relevant transducer parameters
    modS.numTimeSteps = TimeSteps;
    modS.dt = dt;
    modS.numMaterials = numMat;
    modS.numExcitations = numExc;

    modMaterialsList.resize(modS.numMaterials);
    modExcitationsList.resize(modS.numExcitations);
    memcpy(&(modS.inputFile), inFname, sizeof(char)*50); 
    memcpy(&(modS.inputPath), inPname, sizeof(char)*150); 
    return; 
}

void swSim::Model::Copy(Model *other) {
    modS.numTimeSteps = other->modS.numTimeSteps ;
    modS.dt = other->modS.dt;
    modS.numMaterials = other->modS.numMaterials;
    modS.numExcitations = other->modS.numExcitations;

    modMaterialsList.resize(modS.numMaterials);
    modExcitationsList.resize(modS.numExcitations);

    memcpy(&(modS.inputFile), other->modS.inputFile, sizeof(char)*50); 
    memcpy(&(modS.inputPath), other->modS.inputPath, sizeof(char)*150); 
    
    return;    
}

MPI_Datatype swSim::Model::MPICreate() {

    MPI_Datatype data_Types[6] = {MPI_UINT32_T, MPI_DOUBLE, MPI_UINT8_T, MPI_UINT8_T, MPI_CHAR,  MPI_CHAR};
    int blockLengths[6] = {1, 1, 1, 1,  50, 150}; 
    MPI_Aint offsets[6];

    offsets[0] = offsetof(model_s, numTimeSteps);
    offsets[1] = offsetof(model_s, dt);
    offsets[2] = offsetof(model_s, numMaterials);
    offsets[3] = offsetof(model_s, numExcitations);
    offsets[4] = offsetof(model_s, inputFile);
    offsets[5] = offsetof(model_s, inputPath);


    MPI_Type_create_struct(6, blockLengths, offsets, data_Types, &mpi_model);
    MPI_Type_commit(&mpi_model);


    return mpi_model;
}

void swSim::Model::ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber) {
    xmlNodePtr cur2; // A pointer used to navigate the xml file.
    cur2 = cur->xmlChildrenNode;
    xmlNodePtr cur3; // A pointer used to navigate the xml file.

    switch (caseNumber) {
        case 0: {   while (cur2 != NULL){   
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "numTimeSteps")) {
                            Do_parse(doc, cur2, &modS.numTimeSteps);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "dt")) {
                            Do_parse(doc, cur2, &modS.dt);
                        }
                        cur2 = cur2->next;
                    }
                    break;
                }
        case 1: {   modGeometry.Parse(doc, cur);
                    break;
                }
        case 2: {   while (cur2 != NULL){
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "numberMaterials")) {
                            Do_parse(doc, cur2, &modS.numMaterials);
                            modMaterialsList.resize(modS.numMaterials);      
                        }
                        cur2 = cur2->next;
                    }
                    cur3 = cur->xmlChildrenNode;
                    int mat_idx = 0;  
                    while (cur3 != NULL) {
                        if (!xmlStrcmp(cur3->name, (const xmlChar *) "material")) {
                            modMaterialsList.at(mat_idx).Parse(doc, cur3);
                            mat_idx = mat_idx + 1; 
                        }
                        cur3 = cur3->next;
                    }
                    break;
                }
        case 3: {   int exc_idx; 
                    exc_idx = 0;  
                    while (cur2 != NULL){
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "numberExcitations")) {
                            Do_parse(doc, cur2, &modS.numExcitations);
                            modExcitationsList.resize(modS.numExcitations);      
                        }
                        cur2 = cur2->next;
                    }
                    cur3 = cur->xmlChildrenNode;
                    while (exc_idx < modS.numExcitations) {
                        if (!xmlStrcmp(cur3->name, (const xmlChar *) "excitation")) {
                            modExcitationsList[exc_idx].Parse(doc, cur3);
                            exc_idx = exc_idx + 1; 
                        }
                        cur3 = cur3->next;
                    }
                    break;
                }
        default: {  break;}
        }
    return; 
}

void swSim::Model::Input(char *inputFileNameAndPath, MPI_Comm m_mpi_comm) {
    int rank; 
    MPI_Comm_rank(m_mpi_comm, &rank);

    if(rank == 0) {
        readInput(inputFileNameAndPath);
        // std::printf("Rank %d, sending input.\n", rank);
        sendInput(m_mpi_comm);
   }
   else {
        // std::printf("Rank %d, receiving input.\n", rank);
        recieveInput(m_mpi_comm);
   }
   MPI_Barrier(m_mpi_comm);
}

std::string swSim::Model::parsePath(char *inputFileNameAndPath) {
    std::string pathAndFile = inputFileNameAndPath;
    std::string path;
    path = pathAndFile.substr(0, pathAndFile.rfind("/")+1);
    return path;
}

std::string swSim::Model::parseFile(char *inputFileNameAndPath) {
    std::string pathAndFile = inputFileNameAndPath;
    std::string file;
    file = pathAndFile.substr(pathAndFile.rfind("/")+1);
    return file;
}

int swSim::Model::readInput(char *inputFileNameAndPath) {
    xmlDocPtr doc;
    xmlNodePtr  cur;
    doc = xmlParseFile(inputFileNameAndPath);

    std::string path;
    std::string file;
    path = parsePath(inputFileNameAndPath);
    file = parseFile(inputFileNameAndPath);
    memcpy(&(modS.inputFile), file.c_str(), sizeof(char)*50); 
    memcpy(&(modS.inputPath), path.c_str(), sizeof(char)*150); 
    

    if (doc == NULL ) {
        std::printf("Error openning %s.\n", inputFileNameAndPath); 
        return -1; 
    }
    cur = xmlDocGetRootElement(doc);
    
    if (cur == NULL) {
        std::printf("%s is Empty.\n", inputFileNameAndPath); 
        xmlFreeDoc(doc);
        return -1;
    }
    while (cur != NULL){
        if (!xmlStrcmp(cur->name, (const xmlChar *) "simulation")){
            Parse(doc, cur);
        }
        cur = cur->next; 
    }
    xmlFreeDoc(doc);
    return 0; 
}

void swSim::Model::sendInput(MPI_Comm l_mpi_comm) {
    MPI_Status status;
    MPI_Datatype mpi_mod_type;
    MPI_Datatype mpi_geometry_type;
    MPI_Datatype mpi_material_type;
    MPI_Datatype mpi_excitation_type;

    mpi_mod_type = MPICreate(); 
    MPI_Bcast(&modS, 1, mpi_mod_type, 0, l_mpi_comm); 

    mpi_geometry_type = modGeometry.MPICreate();
    MPI_Bcast(&(modGeometry.geomS), 1, mpi_geometry_type, 0, l_mpi_comm);

    for (int mat_idx = 0; mat_idx < modS.numMaterials; mat_idx++) {
        mpi_material_type = modMaterialsList[mat_idx].MPICreate();
        MPI_Bcast(&(modMaterialsList[mat_idx].matS), 1, mpi_material_type, 0, l_mpi_comm); 
    }
    mpi_excitation_type = modExcitationsList[0].MPICreate();
    for (int i = 0; i < modS.numExcitations; i++) {
        MPI_Bcast(&(modExcitationsList[i].excS), 1, mpi_excitation_type, 0, l_mpi_comm); 
    }
    return;
}

void swSim::Model::recieveInput(MPI_Comm l_mpi_comm) {
    int rank; 
    MPI_Comm_rank(l_mpi_comm, &rank);

    MPI_Status status;
    MPI_Datatype mpi_mod_type;
    MPI_Datatype mpi_geometry_type;
    MPI_Datatype mpi_material_type;
    MPI_Datatype mpi_excitation_type;

    swSim::Model lmod; 
    mpi_mod_type = MPICreate(); 
    MPI_Bcast(&lmod.modS, 1, mpi_mod_type, 0, l_mpi_comm);
    this->Copy(&lmod);

    mpi_geometry_type = modGeometry.MPICreate();
    MPI_Bcast(&(modGeometry.geomS), 1, mpi_geometry_type, 0, l_mpi_comm);

    for (int mat_idx = 0; mat_idx < modS.numMaterials; mat_idx++) {
        mpi_material_type = modMaterialsList[mat_idx].MPICreate();
        MPI_Bcast(&(modMaterialsList[mat_idx].matS), 1, mpi_material_type, 0, l_mpi_comm); 
    }

    for (int exc_idx = 0; exc_idx < modS.numExcitations; exc_idx++) {
        mpi_excitation_type = modExcitationsList[exc_idx].MPICreate();
        MPI_Bcast(&(modExcitationsList[exc_idx].excS), 1, mpi_excitation_type, 0, l_mpi_comm); 
    }
    return;
}

void swSim::Model::Print(int rank) {
    std::printf("Rank %d, Time Steps %d.\n", rank, modS.numTimeSteps);
    std::printf("Rank %d, time step size %.10f.\n", rank, modS.dt);
    std::printf("Rank %d, Number of Materials %d.\n", rank, modS.numMaterials);
    std::printf("Rank %d, Number of Excitations %d.\n", rank, modS.numExcitations);
    
    for (int i = 0; i < 3; i++) {
            std::printf("Rank %d, Voxel Dimension %d = %d.\n", rank, i, modGeometry.geomS.dimensionsGlobalVoxel[i]); 
    }
    std::printf("Rank %d, Voxel Size %f x %f x %f mm.\n", rank, modGeometry.geomS.voxelSize[0]*1000, modGeometry.geomS.voxelSize[1]*1000, modGeometry.geomS.voxelSize[2]*1000 );
    std::printf("Rank %d, Input File Name %s.\n", rank, modS.inputFile);
    std::printf("Rank %d, Input File Path %s.\n", rank, modS.inputPath);
    std::printf("Rank %d, Geometry file %s.\n", rank, modGeometry.geomS.geometryFile);
    std::printf("Rank %d, Rotation file %s.\n", rank, modGeometry.geomS.rotationsFile);

    for (int mat_i = 0; mat_i<modS.numMaterials; mat_i++) {
        std::printf("Rank %d, Material %d, Material ID %d.\n", rank, mat_i, modMaterialsList[mat_i].matS.materialID); 
        std::printf("Rank %d, Material %d, Material density %f.\n", rank, mat_i, modMaterialsList[mat_i].matS.density); 
        for (int i = 0; i < 21; i++) {
            std::printf("Rank %d, Material %d, Material Stiffness Value %d = %f.\n", rank, mat_i, i, modMaterialsList[mat_i].matS.stiffnessValues[i]); 
        }
    }

    for (int exc_i = 0; exc_i < modS.numExcitations; exc_i ++) {
        std::printf("Rank %d, Excitation %d, Excitation ID %d.\n", rank, exc_i, modExcitationsList[exc_i].excS.excitationID); //
        std::printf("Rank %d, Excitation %d, Excitation radius %f.\n", rank, exc_i, modExcitationsList[exc_i].excS.radius); //
        std::printf("Rank %d, Excitation %d, Located at (%f, %f, %f).\n", rank, exc_i, modExcitationsList[exc_i].excS.location[0], modExcitationsList[exc_i].excS.location[1], modExcitationsList[exc_i].excS.location[2]);  
    }
    return; 

}

