//system imports
#include <vector>

//GL imports
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "maths_funcs.h"

//project imports
#include "LoadModel.h"

ModelData loadModelMesh(static const char* meshName) {
	
	ModelData modelData;
	//use the assmip function aiImport file to load the given model using the input meshName
	const aiScene* scene = aiImportFile(
		meshName,
		aiProcess_Triangulate |
		aiProcess_OptimizeMeshes |
		aiProcess_GenSmoothNormals |
		aiProcess_ImproveCacheLocality |
		aiProcess_CalcTangentSpace |
		aiProcess_PreTransformVertices
	);
	//print an error log in case of error during import
	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", meshName);
		return modelData;
	}
	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
		}
	}

	aiReleaseImport(scene);
	return modelData;
}