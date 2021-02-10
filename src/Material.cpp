#include "Material.hpp"

swSim::Material::Material()
{
    parseNames.clear();
    parseNames.push_back("number");
    parseNames.push_back("density");
    parseNames.push_back("stiffnessMatrix");

    matS.materialID = 0;
    matS.density = 0.0; 

    for(int i = 0; i < 21; i++)
    {
        matS.stiffnessValues[i] = 0.0;
    }
    return; 
}

swSim::Material::~Material() {}

void swSim::Material::Init(uint32_t id, float rho, float *c) {
    //assign relevant transducer parameters
    matS.materialID = id;
    matS.density = rho; 

    for(int i = 0; i < 21; i++)
    {
        matS.stiffnessValues[i] = c[i];
    }
    return; 
}

void swSim::Material::Copy(swSim::Material *other) {
    //assign relevant transducer parameters
    matS.materialID = other->matS.materialID;
    matS.density = other->matS.density; 

    for(int i = 0; i < 21; i++){
        matS.stiffnessValues[i] = other->matS.stiffnessValues[i];
    }
    return;
}

MPI_Datatype swSim::Material::MPICreate() {
    MPI_Datatype data_Types[3] = {MPI_UINT32_T, MPI_FLOAT, MPI_FLOAT};
    int blockLengths[3] = {1, 1, 21}; 
    MPI_Aint offsets[3];
    offsets[0] = offsetof(material_s, materialID);
    offsets[1] = offsetof(material_s, density);
    offsets[2] = offsetof(material_s, stiffnessValues);
    MPI_Type_create_struct(3, blockLengths, offsets, data_Types, &mpi_material);
    MPI_Type_commit(&mpi_material);
    return mpi_material;
} 


void swSim::Material::ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber) {
    //cur = cur->xmlChildrenNode; 
    xmlNodePtr cur2; // A pointer used to navigate the xml file.

    switch (caseNumber) {
        case 0: {   Do_parse(doc, cur, &matS.materialID);
                    break;
                }
        case 1: {   Do_parse(doc, cur, &matS.density);
                    break;
                }
        case 2: {   cur2 = cur->xmlChildrenNode;
                    int c_idx = 0; 
                    while (c_idx < 21){
                        if ((xmlStrcmp(cur2->name, (const xmlChar *) "c0")>0) && (!xmlNodeIsText(cur2))) {
                            Do_parse(doc, cur2, &matS.stiffnessValues[c_idx]);
                            c_idx = c_idx+1; 
                        }
                        cur2 = cur2->next;
                    }
                    break;
                }
        
        default: {  break;}
        }
    return; 
}

void swSim::Material::RotateStiffnessMatrix(float *rotVec, float *Cout) {
    Eigen::Matrix3f R = get3DRotation(rotVec); 
    Matrix6f C = StiffnessValuesArrayToMatrix();
    Matrix6f M = get3DTransformationMatrix(R);
    Matrix6f Crot = M*C*M.transpose();
    MatrixToStiffnessValuesArray(&Cout[0], Crot); 
    return; 
}

Eigen::Matrix3f swSim::Material::getRotationAboutZ(float angle) {
    Eigen::Matrix3f R;
    R << cos(angle),  -sin(angle),  0,
        sin(angle),  cos(angle),    0,
        0,           0,             1;

    return R; 
}

Eigen::Matrix3f swSim::Material::getRotationAboutY(float angle) {
    Eigen::Matrix3f R;
    R << cos(angle),    0,               sin(angle),
        0,              1,               0,
        -sin(angle),    0,               cos(angle);

    return R; 
}

Eigen::Matrix3f swSim::Material::getRotationAboutX(float angle) {
    Eigen::Matrix3f R;
    R << 1,              0,           0,
        0,               cos(angle),  -sin(angle),
        0,               sin(angle),   cos(angle);
    return R; 
}

Eigen::Matrix3f swSim::Material::get3DRotation(float *rotVec) {
    Eigen::Matrix3f Rz = getRotationAboutZ(rotVec[0]);
    Eigen::Matrix3f Ry = getRotationAboutY(rotVec[1]);
    Eigen::Matrix3f Rx = getRotationAboutX(rotVec[2]);
    return Rz*Ry*Rx;
}

Matrix6f swSim::Material::get3DTransformationMatrix(Eigen::Matrix3f R) {
    Matrix6f M; 
    M << R(0,0)*R(0,0),  R(0,1)*R(0,1),  R(0,2)*R(0,2), 2*R(0,1)*R(0,2),                2*R(0,0)*R(0,2),                2*R(0,0)*R(0,1),
         R(1,0)*R(1,0),  R(1,1)*R(1,1),  R(1,2)*R(1,2), 2*R(1,1)*R(1,2),                2*R(1,0)*R(1,2),                2*R(1,0)*R(1,1),
         R(2,0)*R(2,0),  R(2,1)*R(2,1),  R(2,2)*R(2,2), 2*R(2,1)*R(2,2),                2*R(2,0)*R(2,2),                2*R(2,0)*R(2,1),
         R(1,0)*R(2,0),  R(1,1)*R(2,1),  R(1,2)*R(2,2), R(1,1)*R(2,2)+ R(2,1)*R(1,2),   R(1,0)*R(2,2)+ R(1,2)*R(2,0),   R(1,0)*R(2,1)+ R(1,1)*R(2,0),
         R(0,0)*R(2,0),  R(0,1)*R(2,1),  R(0,2)*R(2,2), R(0,1)*R(2,2)+ R(0,2)*R(2,1),   R(0,0)*R(2,2)+ R(0,2)*R(2,0),   R(0,0)*R(2,1)+ R(0,1)*R(2,0),
         R(0,0)*R(1,0),  R(0,1)*R(1,1),  R(0,2)*R(1,2), R(0,1)*R(1,2)+ R(0,2)*R(1,1),   R(0,0)*R(1,2)+ R(0,2)*R(1,0),   R(0,0)*R(1,1)+ R(0,1)*R(1,0);
    return M; 
}

Matrix6f swSim::Material::StiffnessValuesArrayToMatrix() {
    Matrix6f C; 
    int arrayIndex = 0; 
    for(int i = 0; i<6; i++) 
    {
        for(int j=i; j<6; j++) 
        {
            C(i,j) = matS.stiffnessValues[arrayIndex];
            C(j,i) = matS.stiffnessValues[arrayIndex];
            arrayIndex++; 
        }
    }
    return C; 
}

void swSim::Material::MatrixToStiffnessValuesArray(float *valueArray, Matrix6f C) {
    for(int i = 0; i<6; i++) {
        for(int j=0; j<6; j++) {
            valueArray[i*6+j] = C(i,j);
        }

    }
    return; 
}