//

#include <string>
#include <vector>
#include <cstdio>

#include "Config.hpp"
#include "Geometry.hpp"

#include "gtest/gtest.h"

class GeometryTest: public ::testing::Test{
    protected:
    //DenseVectorTest(){};
    //~DenseVectorTest(){};
    
    void SetUp() override{
        inputFile = std::string(testInputFileDirectory) + std::string("30x30ElongatedQausiIsoCFRPTestPlate/") + std::string("QuasiIsoPlateSimulationInputs.xml");
        doc = xmlParseFile(inputFile.c_str());
        cur = xmlDocGetRootElement(doc);

        dimensions.clear();
        dimensions.push_back(122);
        dimensions.push_back(122);
        dimensions.push_back(26);
        voxSz.clear();
        voxSz.push_back(0.00025);
        voxSz.push_back(0.00025);
        voxSz.push_back(0.00004);
        geomfile = "Geometry";
        rotfile = "Rotations.bin";
        geomParseNames.clear();
        geomParseNames.push_back("numVoxels");
        geomParseNames.push_back("voxelSize");
        geomParseNames.push_back("geometryFileName");
        geomParseNames.push_back("rotations");
    };
    //void TearDown() override{};
    xmlDocPtr doc;
    xmlNodePtr cur;
    std::string inputFile;

    std::vector<double> voxSz;
    std::vector<uint64_t> dimensions;
    std::string geomfile; 
    std::string rotfile;
    std::vector<std::string> geomParseNames;  
};

TEST_F(GeometryTest,Initializer){
    swSim::Geometry geom;
    geom.Init(&dimensions[0], &voxSz[0], geomfile.c_str(), rotfile.c_str());
    EXPECT_EQ(geom.geomS.dimensionsGlobalVoxel[0], dimensions[0]);
    EXPECT_EQ(geom.geomS.dimensionsGlobalVoxel[1], dimensions[1]);
    EXPECT_EQ(geom.geomS.dimensionsGlobalVoxel[2], dimensions[2]);
    EXPECT_EQ(geom.geomS.voxelSize[0], voxSz[0]);
    EXPECT_EQ(geom.geomS.voxelSize[1], voxSz[1]);
    EXPECT_EQ(geom.geomS.voxelSize[2], voxSz[2]);
    EXPECT_STREQ(geom.geomS.geometryFile, geomfile.c_str());
    EXPECT_STREQ(geom.geomS.rotationsFile, rotfile.c_str());

    for(int i = 0; i < geomParseNames.size(); i++) {
        EXPECT_EQ(geom.parseNames[i], geomParseNames[i]); 
    }  
};
TEST_F(GeometryTest,Initializer2){
    swSim::Geometry geom;
    geom.Init(&dimensions[0], voxSz[0], geomfile.c_str(), rotfile.c_str());
    EXPECT_EQ(geom.geomS.dimensionsGlobalVoxel[0], dimensions[0]);
    EXPECT_EQ(geom.geomS.dimensionsGlobalVoxel[1], dimensions[1]);
    EXPECT_EQ(geom.geomS.dimensionsGlobalVoxel[2], dimensions[2]);
    EXPECT_EQ(geom.geomS.voxelSize[0], voxSz[0]);
    EXPECT_EQ(geom.geomS.voxelSize[1], voxSz[0]);
    EXPECT_EQ(geom.geomS.voxelSize[2], voxSz[0]);
    EXPECT_STREQ(geom.geomS.geometryFile, geomfile.c_str());
    EXPECT_STREQ(geom.geomS.rotationsFile, rotfile.c_str());

    for(int i = 0; i < geomParseNames.size(); i++) {
        EXPECT_EQ(geom.parseNames[i], geomParseNames[i]); 
    }  
};
TEST_F(GeometryTest,Copier){
    swSim::Geometry geom1;
    swSim::Geometry geom2;
    geom1.Init(&dimensions[0], &voxSz[0], geomfile.c_str(), rotfile.c_str());
    geom2.Copy(&geom1);
    EXPECT_EQ(geom2.geomS.dimensionsGlobalVoxel[0], dimensions[0]);
    EXPECT_EQ(geom2.geomS.dimensionsGlobalVoxel[1], dimensions[1]);
    EXPECT_EQ(geom2.geomS.dimensionsGlobalVoxel[2], dimensions[2]);
    EXPECT_EQ(geom2.geomS.voxelSize[0], voxSz[0]);
    EXPECT_EQ(geom2.geomS.voxelSize[1], voxSz[1]);
    EXPECT_EQ(geom2.geomS.voxelSize[2], voxSz[2]);
    EXPECT_STREQ(geom2.geomS.geometryFile, geomfile.c_str());
    EXPECT_STREQ(geom2.geomS.rotationsFile, rotfile.c_str());
    
};

TEST_F(GeometryTest,FileNamesGetter){
    swSim::Geometry geom;
    std::string pcFilename = "./Geometry_planeCounts.raw";
    std::string geomFilename = "./Geometry.raw";
    std::string rotFilename = "./Rotations.bin";
    geom.Init(&dimensions[0], &voxSz[0], geomfile.c_str(), rotfile.c_str());
    EXPECT_EQ(geom.GetGeometryFile("./"), geomFilename);
    EXPECT_EQ(geom.GetPlaneCountFile("./"), pcFilename);
    EXPECT_EQ(geom.GetRotationsFile("./"), rotFilename);  
};

TEST_F(GeometryTest, ParseSearchTest){
    swSim::Geometry geom;
    cur = xmlDocGetRootElement(doc);
    cur = geom.ParseSearch(doc, cur, "geometry");
    xmlNodePtr cur2;
    char keystr[50]; // The xml tag character container
    for (int i = 0; i < geomParseNames.size(); i++) {
        cur2 = geom.ParseSearch(doc, cur, geomParseNames[i].c_str());
        sprintf(keystr, "%s", cur2->name);
        EXPECT_STREQ(keystr, geomParseNames[i].c_str());
    }
};

TEST_F(GeometryTest,DoParseTest){
    xmlNodePtr cur2;
    swSim::Geometry geom;
    xmlNodePtr cur3;

    cur = xmlDocGetRootElement(doc);
    cur2 = geom.ParseSearch(doc, cur, "geometry");
    cur2 = cur2->xmlChildrenNode;
    while (cur2 != NULL)
    {   
        if (!xmlStrcmp(cur2->name, (const xmlChar *) "numVoxels")) 
        {
            cur3 = cur2->xmlChildrenNode;
            while (cur3 != NULL){
                if (!xmlStrcmp(cur3->name, (const xmlChar *) "X")) {
                    geom.Do_parse(doc, cur3, &geom.geomS.dimensionsGlobalVoxel[0]);
                }
                if (!xmlStrcmp(cur3->name, (const xmlChar *) "Y")) {
                    geom.Do_parse(doc, cur3, &geom.geomS.dimensionsGlobalVoxel[1]);
                }
                if (!xmlStrcmp(cur3->name, (const xmlChar *) "Z")) {
                    geom.Do_parse(doc, cur3, &geom.geomS.dimensionsGlobalVoxel[2]);
                }
                cur3 = cur3->next;
            }
        }
        if (!xmlStrcmp(cur2->name, (const xmlChar *) "voxelSize")) 
        {
            cur3 = cur2->xmlChildrenNode;
            while (cur3 != NULL){
                if (!xmlStrcmp(cur3->name, (const xmlChar *) "X")) {
                    geom.Do_parse(doc, cur3, &geom.geomS.voxelSize[0]);
                }
                if (!xmlStrcmp(cur3->name, (const xmlChar *) "Y")) {
                    geom.Do_parse(doc, cur3, &geom.geomS.voxelSize[1]);
                }
                if (!xmlStrcmp(cur3->name, (const xmlChar *) "Z")) {
                    geom.Do_parse(doc, cur3, &geom.geomS.voxelSize[2]);
                }
                cur3 = cur3->next;
            }
        }
        if (!xmlStrcmp(cur2->name, (const xmlChar *) "geometryFileName")) 
        {
            geom.Do_parse(doc, cur2, &geom.geomS.geometryFile[0]);
        }
        if (!xmlStrcmp(cur2->name, (const xmlChar *) "rotations")) 
        {
            cur3 = cur2->xmlChildrenNode;
            while (cur3 != NULL){
                if (!xmlStrcmp(cur3->name, (const xmlChar *) "rotationFileName")) {
                    geom.Do_parse(doc, cur3, &geom.geomS.rotationsFile[0]);
                }
                cur3 = cur3->next;
            }
        }
        cur2 = cur2->next;
    }
    for(int i = 0; i < dimensions.size(); i++) {
        EXPECT_EQ(geom.geomS.dimensionsGlobalVoxel[i], dimensions[i]);
        EXPECT_EQ(geom.geomS.voxelSize[i], voxSz[i]);
    }
    EXPECT_STREQ(geom.geomS.geometryFile, geomfile.c_str());
    EXPECT_STREQ(geom.geomS.rotationsFile, rotfile.c_str());
};

TEST_F(GeometryTest,ParseTest){
    xmlNodePtr cur2;
    swSim::Geometry geom;

    cur = xmlDocGetRootElement(doc);
    cur2 = geom.ParseSearch(doc, cur, "geometry");
    geom.Parse(doc, cur2); 

    for(int i = 0; i < dimensions.size(); i++) {
        EXPECT_EQ(geom.geomS.dimensionsGlobalVoxel[i], dimensions[i]);
        EXPECT_EQ(geom.geomS.voxelSize[i], voxSz[i]);
    }
    EXPECT_STREQ(geom.geomS.geometryFile, geomfile.c_str());
    EXPECT_STREQ(geom.geomS.rotationsFile, rotfile.c_str());
};

