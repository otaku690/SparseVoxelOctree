SparseVoxelOctree 
=================

Sparse Voxel Octree Implementation - Upenn CIS-565 final project

# Current Progress
## Alpha presentation:
 * [slides](doc/alphademo.ppt)

## Scene Voxelization using GPU rasterization
 * Use the method described in [Ch. 22 of OpenGL Insight](http://www.seas.upenn.edu/~pcozzi/OpenGLInsights/OpenGLInsights-SparseVoxelization.pdf).
 * Each triangle is orthographically projected, in geometry shader, along the dominant axis of its plane normal vector.
 * Projection result:
 
 ![projection](doc/shot1.png)  
 * The whole scene can be voxelized in one single draw call.
 * In fragment shader, the geometry data is stored into a 3D texture, using GL_EXT_shader_image_load_store extension introduced in OpengGL 4.2.
 * Voxelized Geometry:  
 
 ![voxeledCow](doc/shot2.png)
 
## Sparse Voxel Octree Construction  
 * Still working on it...
## Voxel Cone Tracing 
 * Still working on it...
 
## References:
 * Cyril Crassin and Simon Green. “Octree-based Sparse Voxelization Using the GPU Hardware Rasterizer.” OpenGL Insights, pp. 303-319.
 * Cyril Crassin, Fabrice Neyret, Miguel Sainz, Simon Green, and Elmar Eisemann. “Interactive indirect illumination using voxel cone tracing.” Pacific Graphics 2011.
 


