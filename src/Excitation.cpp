#include "Excitation.hpp"

swSim::Excitation::Excitation() 
{
    excS.excitationProcess = MPI_PROC_NULL;
    excS.excitationCenterElement = 0;
    excS.excitationEdgeElement = 0; 
    parseNames.clear();
    parseNames.push_back("number");
    parseNames.push_back("location");
    parseNames.push_back("radius");
    parseNames.push_back("driven");
    parseNames.push_back("initTimeStep");
    parseNames.push_back("numTimeSteps");
    parseNames.push_back("waveformFile");
    parseNames.push_back("scalingVector");
    parseNames.push_back("orientation");
}

swSim::Excitation::~Excitation() {}

void swSim::Excitation::Init(uint32_t id, double *pos, double rad, bool drvn, uint32_t tinit, uint32_t drlen, const char *fname, float *sF, float *orient)
{
    //assign relevant transducer parameters
    excS.excitationID = id;
    excS.radius = rad;
    excS.driven = drvn;
    excS.initTimeStep = tinit;
    excS.excitationNumTimeSteps = drlen;

    for(int i = 0; i < 3; i++){
        excS.location[i] = pos[i];
        excS.scalingVector[i] = sF[i];
        excS.orientNormal[i] = orient[i];
    }
    excS.excitationProcess = -1;
    excS.excitationCenterElement = 0; 
    excS.excitationEdgeElement = 0; 
    memcpy(&(excS.waveformFile), fname, sizeof(char)*50); 
    return; 
}

void swSim::Excitation::Copy(Excitation *other) 
{
    excS.excitationID = other->excS.excitationID;
    excS.radius = other->excS.radius;
    excS.driven = other->excS.driven ;
    excS.initTimeStep = other->excS.initTimeStep;
    excS.excitationNumTimeSteps = other->excS.excitationNumTimeSteps;

    for(int i = 0; i < 3; i++){
        excS.location[i] = other->excS.location[i];
        excS.scalingVector[i] = other->excS.scalingVector[i];
        excS.orientNormal[i] = other->excS.orientNormal[i];
    }

    excS.excitationProcess = other->excS.excitationProcess;
    excS.excitationCenterElement = other->excS.excitationCenterElement; 
    excS.excitationEdgeElement = other->excS.excitationEdgeElement; 
    memcpy(&(excS.waveformFile), other->excS.waveformFile, sizeof(char)*50); 
    return; 
}

MPI_Datatype swSim::Excitation::MPICreate() 
{
    MPI_Datatype data_Types[12] = {MPI_UINT32_T, MPI_DOUBLE, MPI_DOUBLE, MPI_C_BOOL, MPI_UINT32_T, MPI_UINT32_T, MPI_CHAR, MPI_FLOAT, MPI_FLOAT, MPI_INT, MPI_UINT64_T, MPI_UINT64_T};
    int blockLengths[12] = {1, 3, 1, 1, 1, 1, 50, 3, 3, 1, 1, 1}; 
    MPI_Aint offsets[12];

    offsets[0] = offsetof(excitation_s, excitationID);
    offsets[1] = offsetof(excitation_s, location);
    offsets[2] = offsetof(excitation_s, radius);
    offsets[3] = offsetof(excitation_s, driven);
    offsets[4] = offsetof(excitation_s, initTimeStep);
    offsets[5] = offsetof(excitation_s, excitationNumTimeSteps);
    offsets[6] = offsetof(excitation_s, waveformFile);
    offsets[7] = offsetof(excitation_s, scalingVector);
    offsets[8] = offsetof(excitation_s, orientNormal);
    offsets[9] = offsetof(excitation_s, excitationProcess);
    offsets[10] = offsetof(excitation_s, excitationCenterElement);
    offsets[11] = offsetof(excitation_s, excitationEdgeElement);

    MPI_Type_create_struct(12, blockLengths, offsets, data_Types, &mpi_excitation);
    MPI_Type_commit(&mpi_excitation);

    return mpi_excitation;
}

void swSim::Excitation::ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber) 
{
    switch (caseNumber) {
        case 0: {   Do_parse(doc, cur, &excS.excitationID);
                    break;
                }
        case 1: {   xmlNodePtr cur2 = cur->xmlChildrenNode;
                    while (cur2 != NULL){
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "X")) {
                            Do_parse(doc, cur2, &excS.location[0]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Y")) {
                            Do_parse(doc, cur2, &excS.location[1]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Z")) {
                            Do_parse(doc, cur2, &excS.location[2]);
                        }
                        cur2 = cur2->next;
                    }
                    break;
                }
        case 2: {   Do_parse(doc, cur, &excS.radius);
                    break;
                }
        case 3: {   Do_parse(doc, cur, &excS.driven);
                    break;
                }
        case 4: {   Do_parse(doc, cur, &excS.initTimeStep);
                    break;
                }
        case 5: {   Do_parse(doc, cur, &excS.excitationNumTimeSteps);
                    break;
                }
        case 6: {   Do_parse(doc, cur, &excS.waveformFile[0]);
                    break;
                }
        case 7: {   xmlNodePtr cur2 = cur->xmlChildrenNode;
                    while (cur2 != NULL){
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "X")) {
                            Do_parse(doc, cur2, &excS.scalingVector[0]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Y")) {
                            Do_parse(doc, cur2, &excS.scalingVector[1]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Z")) {
                            Do_parse(doc, cur2, &excS.scalingVector[2]);
                        }
                        cur2 = cur2->next;
                    }
                    break;
                }
        case 8: {  xmlNodePtr cur2 = cur->xmlChildrenNode;
                    while (cur2 != NULL){
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "X")) {
                            Do_parse(doc, cur2, &excS.orientNormal[0]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Y")) {
                            Do_parse(doc, cur2, &excS.orientNormal[1]);
                        }
                        if (!xmlStrcmp(cur2->name, (const xmlChar *) "Z")) {
                            Do_parse(doc, cur2, &excS.orientNormal[2]);
                        }
                        cur2 = cur2->next;
                    }
                    break;
                }
        default: {  break;}
        }
    return; 
}

bool swSim::Excitation::LocationCheck(std::vector<double> &nCoords) 
{
    double sq_dist = pow(nCoords[0] - (double)excS.location[0], 2)+pow(nCoords[1] - (double)excS.location[1],2)+pow(nCoords[2] - (double)excS.location[2],2);
    return sq_dist < (double)pow(excS.radius,2);
}
