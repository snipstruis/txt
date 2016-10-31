#pragma once
#include <vector>

// REMEMBER: these buffers are all located in graphics memory!
//           the GLuint's are handles to those buffers.

// NOTE ON THE OPENGL GENERIC BUFFER FUNCTIONS
// Ok, so buffers are a little wierd in openGL.
// The whole interface is specified so you can create multiple buffers with
// a single call, so those functions expect to modify an array of handles
// instead of a single handle. We just give them the address of our handle
// and say that it's an array of length 1.


struct mesh{
	GLuint vbo;
	GLuint ibo;
	GLuint vao;
	unsigned short nr_of_indices;
};

mesh create_mesh(const std::vector<GLfloat> vertices,
			     const std::vector<GLushort> indices){
	mesh o;
	// save number of indices so we know how many to draw
	o.nr_of_indices = indices.size();


	//// STEP 1: create Vertex Buffer Object
	//// (a handle to the array of vertices in graphics memory)
	
	// allocate one buffer and put the handle in the struct
	glGenBuffers(1, &o.vbo); 

	// specify that the buffer is to hold vertices
	glBindBuffer(GL_ARRAY_BUFFER, o.vbo);
	
	// copy the data to the graphics memory.
	// since we don't modify the vertices once we have uploaded them, 
	// we use GL_STATIC_DRAW.
	glBufferData(GL_ARRAY_BUFFER,
				 vertices.size() * sizeof(GLfloat),
				 &vertices[0],
				 GL_STATIC_DRAW);
 

	//// STEP 2: create Index Buffer Object handle: 
	//// a handle to the array of indices in graphics memory
	
	// allocate one buffer and put the handle in the struct
	glGenBuffers(1, &o.ibo); 

	// specify that the buffer is to hold vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o.ibo);
	
	// copy the data to the graphics memory.
	// since we don't modify the vertices once we have uploaded them, 
	// we use GL_STATIC_DRAW.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 indices.size() * sizeof(GLushort),
				 &indices[0],
				 GL_STATIC_DRAW);

 
	//// STEP 3: create Vertex Array Object handle:
	//// a handle to structure in graphics memory that holds the handles to the
	//// buffers that the shader program will use.
	
	// allocate one vertexArrayObject
	glGenVertexArrays(1, &o.vao);

	// select vao
	glBindVertexArray(o.vao);	

	glBindBuffer(GL_ARRAY_BUFFER, o.vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o.ibo);

	return o;
}

void destroy_mesh(mesh o){
	glDeleteBuffers(1,&o.vbo);
	glDeleteBuffers(1,&o.ibo);
	glDeleteVertexArrays(1,&o.vao);	
}

void draw_mesh(mesh o){
	glBindVertexArray(o.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o.ibo);
	glDrawElements(GL_TRIANGLES,
				   o.nr_of_indices,
				   GL_UNSIGNED_SHORT,
				   NULL );
}
            
