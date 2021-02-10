//

#include <string>
#include <vector>
#include <cstdio>

#include "Config.hpp"
#include "Material.hpp"

#include "gtest/gtest.h"

class MaterialTest: public ::testing::Test{
    protected:
    //DenseVectorTest(){};
    //~DenseVectorTest(){};
    
    void SetUp() override{
        inputFile = std::string(testInputFileDirectory) + std::string("30x30ElongatedQausiIsoCFRPTestPlate/") + std::string("QuasiIsoPlateSimulationInputs.xml");
        doc = xmlParseFile(inputFile.c_str());
        cur = xmlDocGetRootElement(doc);

        stiffnessValuesVec.clear();
        stiffnessValuesVec.push_back(175200000000.0);
        stiffnessValuesVec.push_back(5940000000.0);
        stiffnessValuesVec.push_back(5940000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(12300000000.0);
        stiffnessValuesVec.push_back(6254000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(12300000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(2800000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(5290000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(5290000000.0);
        matParseNames.clear();
        matParseNames.push_back("number");
        matParseNames.push_back("density");
        matParseNames.push_back("stiffnessMatrix");
    };
    //void TearDown() override{};
    xmlDocPtr doc;
    xmlNodePtr cur;
    std::string inputFile;
    uint32_t IDnumber = 1; 
    float density = 1570.0;
    std::vector<float> stiffnessValuesVec;
    std::vector<std::string> matParseNames; 
};

TEST_F(MaterialTest,Initializer){
    swSim::Material mat;
    mat.Init(IDnumber, density, &stiffnessValuesVec[0]);
    EXPECT_EQ(mat.matS.materialID, IDnumber);
    EXPECT_EQ(mat.matS.density, density);
    for(int i = 0; i<21; i++) 
    {
        EXPECT_EQ(mat.matS.stiffnessValues[i], stiffnessValuesVec[i]);
    }
    for(int i = 0; i<matParseNames.size(); i++) 
    {
       EXPECT_EQ(mat.parseNames[i], matParseNames[i]);  
    }
    
};


TEST_F(MaterialTest,Copier){
    swSim::Material mat1;
    swSim::Material mat2;
    mat1.Init(IDnumber, density, &stiffnessValuesVec[0]);
    mat2.Copy(&mat1);
    EXPECT_EQ(mat2.matS.materialID, IDnumber);
    EXPECT_EQ(mat2.matS.density, density);
    for(int i = 0; i<21; i++) 
    {
        EXPECT_EQ(mat2.matS.stiffnessValues[i], stiffnessValuesVec[i]);
    }
};

TEST_F(MaterialTest,RotateX90){
    swSim::Material mat1;
    mat1.Init(IDnumber, density, &stiffnessValuesVec[0]);
    std::vector<float> OutC(36, 0);
    std::vector<float> rotV;
    rotV.push_back(0.0);
    rotV.push_back(0.0);
    rotV.push_back(3.141592653589793/2.0);
    mat1.RotateStiffnessMatrix(&rotV[0], &OutC[0]);
    EXPECT_NEAR(OutC[0], 175200000000.0, 10000);
    EXPECT_NEAR(OutC[1], 5940000000.0, 1000);
    EXPECT_NEAR(OutC[2], 5940000000.0, 1000);
    EXPECT_NEAR(OutC[3], 0.0, 100000);
    EXPECT_NEAR(OutC[4], 0.0, 100000);
    EXPECT_NEAR(OutC[5], 0.0, 100000);
    EXPECT_NEAR(OutC[6], 5940000000.0, 1000);
    EXPECT_NEAR(OutC[7], 12300000000.0, 1000);
    EXPECT_NEAR(OutC[8], 6254000000.0, 1000);
    EXPECT_NEAR(OutC[9], 0.0, 1000);
    EXPECT_NEAR(OutC[10], 0.0, 1000);
    EXPECT_NEAR(OutC[11], 0.0, 1000);
    EXPECT_NEAR(OutC[12], 5940000000.0, 1000);
    EXPECT_NEAR(OutC[13], 6254000000.0, 1000);
    EXPECT_NEAR(OutC[14], 12300000000.0, 1000);
    EXPECT_NEAR(OutC[15], 0.0, 1000);
    EXPECT_NEAR(OutC[16], 0.0, 1000);
    EXPECT_NEAR(OutC[17], 0.0, 1000);
    EXPECT_NEAR(OutC[18], 0.0, 1000);
    EXPECT_NEAR(OutC[19], 0.0, 1000);
    EXPECT_NEAR(OutC[20], 0.0, 1000);
    EXPECT_NEAR(OutC[21], 2800000000.000000, 1000);
    EXPECT_NEAR(OutC[22], 0.0, 1000);
    EXPECT_NEAR(OutC[23], 0.0, 1000);
    EXPECT_NEAR(OutC[24], 0.0, 1000);
    EXPECT_NEAR(OutC[25], 0.0, 1000);
    EXPECT_NEAR(OutC[26], 0.0, 1000);
    EXPECT_NEAR(OutC[27], 0.0, 1000);
    EXPECT_NEAR(OutC[28], 5290000000.000000, 1000);
    EXPECT_NEAR(OutC[29], 0.0, 1000);
    EXPECT_NEAR(OutC[30], 0.0, 1000);
    EXPECT_NEAR(OutC[31], 0.0, 1000);
    EXPECT_NEAR(OutC[32], 0.0, 1000);
    EXPECT_NEAR(OutC[33], 0.0, 1000);
    EXPECT_NEAR(OutC[34], 0.0, 1000);
    EXPECT_NEAR(OutC[35], 5290000000.000000, 1000);
};

TEST_F(MaterialTest,RotateZ90){
    swSim::Material mat1;
    mat1.Init(IDnumber, density, &stiffnessValuesVec[0]);
    std::vector<float> OutC(36, 0);
    std::vector<float> rotV;
    float err = 10000.0;
    rotV.push_back(3.141592653589793/2.0); 
    rotV.push_back(0.0);
    rotV.push_back(0.0);
    mat1.RotateStiffnessMatrix(&rotV[0], &OutC[0]);
    EXPECT_NEAR(OutC[0], 12300000000.000000, err);
    EXPECT_NEAR(OutC[1], 5940000000.000000, err);
    EXPECT_NEAR(OutC[2], 6254000000.000000, err);
    EXPECT_NEAR(OutC[3], 0.000000, err);
    EXPECT_NEAR(OutC[4], 0.000000, err);
    EXPECT_NEAR(OutC[5], 0.000000, err);
    EXPECT_NEAR(OutC[6], 5940000000.000000, err);
    EXPECT_NEAR(OutC[7], 175200000000.000000, err);
    EXPECT_NEAR(OutC[8], 5940000000.000000, err);
    EXPECT_NEAR(OutC[9], 0.000000, err);
    EXPECT_NEAR(OutC[10], 0.000000, err);
    EXPECT_NEAR(OutC[11], 0.000010, err);
    EXPECT_NEAR(OutC[12], 6254000000.000000, err);
    EXPECT_NEAR(OutC[13], 5940000000.000000, err);
    EXPECT_NEAR(OutC[14], 12300000000.000000, err);
    EXPECT_NEAR(OutC[15], 0.000000, err);
    EXPECT_NEAR(OutC[16], 0.000000, err);
    EXPECT_NEAR(OutC[17], -0.000000, err);
    EXPECT_NEAR(OutC[18], 0.000000, err);
    EXPECT_NEAR(OutC[19], 0.000000, err);
    EXPECT_NEAR(OutC[20], 0.000000, err);
    EXPECT_NEAR(OutC[21], 5290000000.000000, err);
    EXPECT_NEAR(OutC[22], 0.000000, err);
    EXPECT_NEAR(OutC[23], 0.000000, err);
    EXPECT_NEAR(OutC[24], 0.000000, err);
    EXPECT_NEAR(OutC[25], 0.000000, err);
    EXPECT_NEAR(OutC[26], 0.000000, err);
    EXPECT_NEAR(OutC[27], 0.000000, err);
    EXPECT_NEAR(OutC[28], 2800000000.000000, err);
    EXPECT_NEAR(OutC[29], 0.000000, err);
    EXPECT_NEAR(OutC[30], 0.000000, err);
    EXPECT_NEAR(OutC[31], 0.000010, err);
    EXPECT_NEAR(OutC[32], -0.000000, err);
    EXPECT_NEAR(OutC[33], 0.000000, err);
    EXPECT_NEAR(OutC[34], 0.000000, err);
    EXPECT_NEAR(OutC[35], 5290000000.000000, err);
};

TEST_F(MaterialTest,RotateY90){
    swSim::Material mat1;
    mat1.Init(IDnumber, density, &stiffnessValuesVec[0]);
    std::vector<float> OutC(36, 0);
    std::vector<float> rotV;
    float err = 10000.0;
    rotV.push_back(0.0);
    rotV.push_back(3.141592653589793/2.0); 
    rotV.push_back(0.0);
    mat1.RotateStiffnessMatrix(&rotV[0], &OutC[0]);
    EXPECT_NEAR(OutC[0], 12300000000.000000, err);
    EXPECT_NEAR(OutC[1], 6254000000.000000, err);
    EXPECT_NEAR(OutC[2], 5940000000.000000, err);
    EXPECT_NEAR(OutC[3], 0.000000, err);
    EXPECT_NEAR(OutC[4], -0.000000, err);
    EXPECT_NEAR(OutC[5], 0.000000, err);
    EXPECT_NEAR(OutC[6], 6254000000.000000, err);
    EXPECT_NEAR(OutC[7], 12300000000.000000, err);
    EXPECT_NEAR(OutC[8], 5940000000.000000, err);
    EXPECT_NEAR(OutC[9], 0.000000, err);
    EXPECT_NEAR(OutC[10], 0.000000, err);
    EXPECT_NEAR(OutC[11], 0.000000, err);
    EXPECT_NEAR(OutC[12], 5940000000.000000, err);
    EXPECT_NEAR(OutC[13], 5940000000.000000, err);
    EXPECT_NEAR(OutC[14], 175200000000.000000, err);
    EXPECT_NEAR(OutC[15], 0.000000, err);
    EXPECT_NEAR(OutC[16], -0.000010, err);
    EXPECT_NEAR(OutC[17], 0.000000, err);
    EXPECT_NEAR(OutC[18], 0.000000, err);
    EXPECT_NEAR(OutC[19], 0.000000, err);
    EXPECT_NEAR(OutC[20], 0.000000, err);
    EXPECT_NEAR(OutC[21], 5290000000.000000, err);
    EXPECT_NEAR(OutC[22], 0.000000, err);
    EXPECT_NEAR(OutC[23], -0.000000, err);
    EXPECT_NEAR(OutC[24], -0.000000, err);
    EXPECT_NEAR(OutC[25], 0.000000, err);
    EXPECT_NEAR(OutC[26], -0.000010, err);
    EXPECT_NEAR(OutC[27], 0.000000, err);
    EXPECT_NEAR(OutC[28], 5290000000.000000, err);
    EXPECT_NEAR(OutC[29], 0.000000, err);
    EXPECT_NEAR(OutC[30], 0.000000, err);
    EXPECT_NEAR(OutC[31], 0.000000, err);
    EXPECT_NEAR(OutC[32], 0.000000, err);
    EXPECT_NEAR(OutC[33], -0.000000, err);
    EXPECT_NEAR(OutC[34], 0.000000, err);
    EXPECT_NEAR(OutC[35], 2800000000.000000, err);
};

TEST_F(MaterialTest,RotateY65){
    swSim::Material mat1;
    mat1.Init(IDnumber, density, &stiffnessValuesVec[0]);
    std::vector<float> OutC(36, 0);
    std::vector<float> rotV;
    float err = 10000.0;
    rotV.push_back(0.0);
    rotV.push_back(3.141592653589793/180.0*65); 
    rotV.push_back(0.0);
    mat1.RotateStiffnessMatrix(&rotV[0], &OutC[0]);
    EXPECT_NEAR(OutC[0], 18734737000.727112, err);
    EXPECT_NEAR(OutC[1], 6197917654.720786, err);
    EXPECT_NEAR(OutC[2], 28600212190.304260, err);
    EXPECT_NEAR(OutC[3], 0.000000, err);
    EXPECT_NEAR(OutC[4], -12182984254.737175, err);
    EXPECT_NEAR(OutC[5], 0.000000, err);
    EXPECT_NEAR(OutC[6], 6197917654.720786, err);
    EXPECT_NEAR(OutC[7], 12300000000.000000, err);
    EXPECT_NEAR(OutC[8], 5996082345.279214, err);
    EXPECT_NEAR(OutC[9], 0.000000, err);
    EXPECT_NEAR(OutC[10], 120268977.569680, err);
    EXPECT_NEAR(OutC[11], 0.000000, err);
    EXPECT_NEAR(OutC[12], 28600212190.304260, err);
    EXPECT_NEAR(OutC[13], 5996082345.279214, err);
    EXPECT_NEAR(OutC[14], 123444838618.664368, err);
    EXPECT_NEAR(OutC[15], 0.000000, err);
    EXPECT_NEAR(OutC[16], -50211335637.303581, err);
    EXPECT_NEAR(OutC[17], 0.000000, err);
    EXPECT_NEAR(OutC[18], 0.000000, err);
    EXPECT_NEAR(OutC[19], 0.000000, err);
    EXPECT_NEAR(OutC[20], 0.000000, err);
    EXPECT_NEAR(OutC[21], 4845270574.059741, err);
    EXPECT_NEAR(OutC[22], 0.000000, err);
    EXPECT_NEAR(OutC[23], -953725331.683128, err);
    EXPECT_NEAR(OutC[24], -12182984254.737175, err);
    EXPECT_NEAR(OutC[25], 120268977.569680, err);
    EXPECT_NEAR(OutC[26], -50211335637.303589, err);
    EXPECT_NEAR(OutC[27], 0.000000, err);
    EXPECT_NEAR(OutC[28], 27950212190.304256, err);
    EXPECT_NEAR(OutC[29], 0.000000, err);
    EXPECT_NEAR(OutC[30], 0.000000, err);
    EXPECT_NEAR(OutC[31], 0.000000, err);
    EXPECT_NEAR(OutC[32], 0.000000, err);
    EXPECT_NEAR(OutC[33], -953725331.683128, err);
    EXPECT_NEAR(OutC[34], 0.000000, err);
    EXPECT_NEAR(OutC[35], 3244729425.940259, err);
};

TEST_F(MaterialTest,RotateX25Y65Z40){
    swSim::Material mat1;
    mat1.Init(IDnumber, density, &stiffnessValuesVec[0]);
    std::vector<float> OutC(36, 0);
    std::vector<float> rotV;
    float err = 11000.0;
    rotV.push_back(3.141592653589793/180.0*40); 
    rotV.push_back(3.141592653589793/180.0*65); 
    rotV.push_back(3.141592653589793/180.0*25); 
    mat1.RotateStiffnessMatrix(&rotV[0], &OutC[0]);
    EXPECT_NEAR(OutC[0], 14820850834.190054, err);
    EXPECT_NEAR(OutC[1], 7437229911.455046, err);
    EXPECT_NEAR(OutC[2], 19276625831.869751, err);
    EXPECT_NEAR(OutC[3], -3862371884.531359, err);
    EXPECT_NEAR(OutC[4], -5982424968.799624, err);
    EXPECT_NEAR(OutC[5], 1886470813.330782, err);
    EXPECT_NEAR(OutC[6], 7437229911.455045, err);
    EXPECT_NEAR(OutC[7], 13731087139.915493, err);
    EXPECT_NEAR(OutC[8], 15323843216.866795, err);
    EXPECT_NEAR(OutC[9], -3879288336.418617, err);
    EXPECT_NEAR(OutC[10], -3256620885.478507, err);
    EXPECT_NEAR(OutC[11], 1306067240.442339, err);
    EXPECT_NEAR(OutC[12], 19276625831.869747, err);
    EXPECT_NEAR(OutC[13], 15323843216.866796, err);
    EXPECT_NEAR(OutC[14], 123440664105.511292, err);
    EXPECT_NEAR(OutC[15], -32287328111.860741, err);
    EXPECT_NEAR(OutC[16], -38465644799.260765, err);
    EXPECT_NEAR(OutC[17], 11106312550.419827, err);
    EXPECT_NEAR(OutC[18], -3862371884.531358, err);
    EXPECT_NEAR(OutC[19], -3879288336.418616, err);
    EXPECT_NEAR(OutC[20], -32287328111.860737, err);
    EXPECT_NEAR(OutC[21], 14379954805.543852, err);
    EXPECT_NEAR(OutC[22], 11352914207.968182, err);
    EXPECT_NEAR(OutC[23], -4079348258.023023, err);
    EXPECT_NEAR(OutC[24], -5982424968.799623, err);
    EXPECT_NEAR(OutC[25], -3256620885.478505, err);
    EXPECT_NEAR(OutC[26], -38465644799.260757, err);
    EXPECT_NEAR(OutC[27], 11352914207.968184, err);
    EXPECT_NEAR(OutC[28], 18419702471.973213, err);
    EXPECT_NEAR(OutC[29], -4552722119.392916, err);
    EXPECT_NEAR(OutC[30], 1886470813.330781, err);
    EXPECT_NEAR(OutC[31], 1306067240.442338, err);
    EXPECT_NEAR(OutC[32], 11106312550.419823, err);
    EXPECT_NEAR(OutC[33], -4079348258.023023, err);
    EXPECT_NEAR(OutC[34], -4552722119.392916, err);
    EXPECT_NEAR(OutC[35], 4484041682.674516, err);
};

TEST_F(MaterialTest, ParseSearchTest){
    swSim::Material mat;
    cur = xmlDocGetRootElement(doc);
    cur = mat.ParseSearch(doc, cur, "materials");
    cur = mat.ParseSearch(doc, cur, "material");
    xmlNodePtr cur2;
    char keystr[50]; // The xml tag character container
    for (int i = 0; i < matParseNames.size(); i++) {
        cur2 = mat.ParseSearch(doc, cur, matParseNames[i].c_str());
        sprintf(keystr, "%s", cur2->name);
        EXPECT_STREQ(keystr, matParseNames[i].c_str());
    }
};

TEST_F(MaterialTest, ParseTest){
    xmlNodePtr cur2;
    swSim::Material mat;

    cur = xmlDocGetRootElement(doc);
    cur2 = mat.ParseSearch(doc, cur, "materials");
    cur2 = mat.ParseSearch(doc, cur2, "material");
    mat.Parse(doc, cur2); 

    for(int i = 0; i < stiffnessValuesVec.size(); i++) {
        EXPECT_EQ(mat.matS.stiffnessValues[i], stiffnessValuesVec[i]);
    }
    EXPECT_EQ(mat.matS.density, density);
    EXPECT_EQ(mat.matS.materialID, IDnumber);
};


// TEST_F(DenseVectorTest,GetName){
//     swSim::DenseVector DnV;
//     DnV.setName(vecname);
//     EXPECT_EQ(DnV.getName(),vecname);
// };

// TEST_F(DenseVectorTest,GetVector1){
//     swSim::DenseVector DnV;
//     DnV.setName(vecname);
//     DnV.setVector(vec1);
//     ASSERT_NE(DnV.getValueArray(),nullptr);
//     EXPECT_EQ(DnV.getArrayLength(),Length);
//     for(int k=0;k<Length;k++){
//         EXPECT_EQ(DnV.getValueArray()[k],vec1.at(k));
//     };
// };

// TEST_F(DenseVectorTest,GetVector2){
//     swSim::DenseVector DnV;
//     DnV.setName(vecname);
//     DnV.setVector(vec1.data(),vec1.size());
//     ASSERT_NE(DnV.getValueArray(),nullptr);
//     EXPECT_EQ(DnV.getArrayLength(),Length);
//     for(int k=0;k<Length;k++){
//         EXPECT_EQ(DnV.getValueArray()[k],vec1.at(k));
//     };
// };


