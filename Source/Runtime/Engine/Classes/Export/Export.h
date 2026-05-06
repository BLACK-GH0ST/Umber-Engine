#pragma once


/**********************************************************************************************
*
*  File Name   : Export.h
*
*  Description :
*      For the Extracting of Mesh and Objects in the Engine to any formate
*
**********************************************************************************************/

#pragma once

#include "MeshProcessor.h"
#include <vector>

bool ExportMeshes (const char* filename, std::vector<ExportMesh>& meshes);
