#include "Geometry.hpp"

swSim::Geometry::Geometry() {
    parseNames.clear();
    parseNames.push_back("numVoxels");
    parseNames.push_back("voxelSize");
    parseNames.push_back("geometryFileName");
    parseNames.push_back("rotations");
}

swSim::Geometry::~Geometry() {}

void swSim::Geometry::Init(uint64_t *nVox, double *ds, const char *gFname, const char *rFname){
    for(int i = 0; i < 3; i++){
        geomS.dimensionsGlobalVoxel[i] = nVox[i];
        geomS.voxelSize[i] = ds[i];
    }
    memcpy(&(geomS.geometryFile), gFname, sizeof(char)*50); 
    memcpy(&(geomS.rotationsFile), rFname, sizeof(char)*50); 
    return; 
}

void swSim::Geometry::Init(uint64_t *nVox, double ds, const char *gFname, const char *rFname){
    for(int i = 0; i < 3; i++){
        geomS.dimensionsGlobalVoxel[i] = nVox[i];
        geomS.voxelSize[i] = ds;
    }
    memcpy(&(geomS.geometryFile), gFname, sizeof(char)*50); 
    memcpy(&(geomS.rotationsFile), rFname, sizeof(char)*50); 
    return; 
}

void swSim::Geometry::Copy(Geometry *other) {
    for(int i = 0; i < 3; i++){
        geomS.dimensionsGlobalVoxel[i] = other->geomS.dimensionsGlobalVoxel[i];
        geomS.voxelSize[i] = other->geomS.voxelSize[i];
    }
    memcpy(&(geomS.geometryFile), other->geomS.geometryFile, sizeof(char)*50); 
    memcpy(&(geomS.rotationsFile), other->geomS.rotationsFile, sizeof(char)*50); 
    return; 
}

MPI_Datatype swSim::Geometry::MPICreate(){
    MPI_Datatype data_Types[4] = {MPI_UINT64_T, MPI_DOUBLE, MPI_CHAR, MPI_CHAR};  
    int blockLengths[4] = {3, 3, 50, 50};
    MPI_Aint offsets[4];
    offsets[0] = offsetof(geometry_s, dimensionsGlobalVoxel);
    offsets[1] = offsetof(geometry_s, voxelSize);
    offsets[2] = offsetof(geometry_s, geometryFile);
    offsets[3] = offsetof(geometry_s, rotationsFile);

    MPI_Type_create_struct(4, blockLengths, offsets, data_Types, &mpi_geometry);
    MPI_Type_commit(&mpi_geometry);

    return mpi_geometry; 
}

void swSim::Geometry::ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber){
    xmlNodePtr cur2; // A pointer used to navigate the xml file.
    switch (caseNumber) {
        case 0: {   cur2 = cur->xmlChildrenNode;
                    while (cur2 != NULL){
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "X")) {
                            Do_parse(doc, cur2, &geomS.dimensionsGlobalVoxel[0]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Y")) {
                            Do_parse(doc, cur2, &geomS.dimensionsGlobalVoxel[1]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Z")) {
                            Do_parse(doc, cur2, &geomS.dimensionsGlobalVoxel[2]);
                        }
                        cur2 = cur2->next;
                    }
                    break;
                }
        case 1: {   cur2 = cur->xmlChildrenNode;
                    Do_parse(doc, cur, &geomS.voxelSize[0]);
                    Do_parse(doc, cur, &geomS.voxelSize[1]);
                    Do_parse(doc, cur, &geomS.voxelSize[2]);
                    while (cur2 != NULL){
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "X")) {
                            Do_parse(doc, cur2, &geomS.voxelSize[0]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Y")) {
                            Do_parse(doc, cur2, &geomS.voxelSize[1]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Z")) {
                            Do_parse(doc, cur2, &geomS.voxelSize[2]);
                        }
                        cur2 = cur2->next;
                    }


                    break;
                }
        case 2: {   Do_parse(doc, cur, &geomS.geometryFile[0]);
                    break;
                }
        case 3: {   cur2 = cur->xmlChildrenNode;
                    while (cur2 != NULL){
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "rotationFileName")) {
                            Do_parse(doc, cur2, &geomS.rotationsFile[0]);
                        }
                        cur2 = cur2->next;
                    }
                    break;
                }
        default: {  break;}
        }
    return; 
}

std::string swSim::Geometry::GetGeometryFile(std::string path) {
    std::string geomFile = path + std::string(geomS.geometryFile)+ ".raw";
    return geomFile;
}

std::string swSim::Geometry::GetPlaneCountFile(std::string path) {
    std::string plnCntFile = path + std::string(geomS.geometryFile)+ "_planeCounts.raw";
    return plnCntFile;
}

std::string swSim::Geometry::GetRotationsFile(std::string path) {
    std::string rotFile = path + std::string(geomS.rotationsFile);
    return rotFile;
}
