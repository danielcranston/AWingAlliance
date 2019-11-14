// from https://github.com/syoyo/tinyobjloader

namespace // unnamed implementation-dependent functions not to be exposed outwards
{
	bool load_obj(tinyobj::attrib_t *attrib,
					std::vector<tinyobj::shape_t> *shapes,
					std::vector<tinyobj::material_t> *materials,
					const std::string &model_name)
    {
		std::string warn;
		std::string err;
		std::string base_dir = "Models/";
		std::string filename = base_dir + model_name + ".obj";
    	bool ret = tinyobj::LoadObj(attrib, shapes, materials, &warn, &err, filename.c_str(), base_dir.c_str());
		if (!warn.empty())
			std::cout << "WARN: " << warn << std::endl;
		if (!err.empty())
			std::cerr << err << std::endl;
		if(!ret)
		{
			std::string what = "Unable to load obj: " + filename;
			exit(1);
		}
		return ret;
    }

    bool FileExists(const std::string& abs_filename)
	{
		bool ret;
		FILE* fp = fopen(abs_filename.c_str(), "rb");
		if (fp) {
			ret = true;
			fclose(fp);
		} else {
			ret = false;
		}
		return ret;
	}

	static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
		float v10[3];
		v10[0] = v1[0] - v0[0];
		v10[1] = v1[1] - v0[1];
		v10[2] = v1[2] - v0[2];

		float v20[3];
		v20[0] = v2[0] - v0[0];
		v20[1] = v2[1] - v0[1];
		v20[2] = v2[2] - v0[2];

		N[0] = v20[1] * v10[2] - v20[2] * v10[1];
		N[1] = v20[2] * v10[0] - v20[0] * v10[2];
		N[2] = v20[0] * v10[1] - v20[1] * v10[0];

		float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
		if (len2 > 0.0f) {
			float len = sqrtf(len2);

			N[0] /= len;
			N[1] /= len;
			N[2] /= len;
		}
	}

	struct vec3 {
		float v[3];
		vec3() {
			v[0] = 0.0f;
			v[1] = 0.0f;
			v[2] = 0.0f;
		}
	};

	void normalizeVector(vec3 &v) {
		float len2 = v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
		if (len2 > 0.0f) {
			float len = sqrtf(len2);

			v.v[0] /= len;
			v.v[1] /= len;
			v.v[2] /= len;
		}
	}

	// Check if `mesh_t` contains smoothing group id.
	bool hasSmoothingGroup(const tinyobj::shape_t& shape)
	{
		for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++) {
			if (shape.mesh.smoothing_group_ids[i] > 0) {
				return true;
			}
		}
		return false;
	}

	void computeSmoothingNormals(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape,
															 std::map<int, vec3>& smoothVertexNormals) {
		smoothVertexNormals.clear();
		std::map<int, vec3>::iterator iter;

		for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {
			// Get the three indexes of the face (all faces are triangular)
			tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
			tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
			tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

			// Get the three vertex indexes and coordinates
			int vi[3];      // indexes
			float v[3][3];  // coordinates

			for (int k = 0; k < 3; k++) {
				vi[0] = idx0.vertex_index;
				vi[1] = idx1.vertex_index;
				vi[2] = idx2.vertex_index;
				assert(vi[0] >= 0);
				assert(vi[1] >= 0);
				assert(vi[2] >= 0);

				v[0][k] = attrib.vertices[3 * vi[0] + k];
				v[1][k] = attrib.vertices[3 * vi[1] + k];
				v[2][k] = attrib.vertices[3 * vi[2] + k];
			}

			// Compute the normal of the face
			float normal[3];
			CalcNormal(normal, v[0], v[1], v[2]);

			// Add the normal to the three vertexes
			for (size_t i = 0; i < 3; ++i) {
				iter = smoothVertexNormals.find(vi[i]);
				if (iter != smoothVertexNormals.end()) {
					// add
					iter->second.v[0] += normal[0];
					iter->second.v[1] += normal[1];
					iter->second.v[2] += normal[2];
				} else {
					smoothVertexNormals[vi[i]].v[0] = normal[0];
					smoothVertexNormals[vi[i]].v[1] = normal[1];
					smoothVertexNormals[vi[i]].v[2] = normal[2];
				}
			}

		}  // f

		// Normalize the normals, that is, make them unit vectors
		for (iter = smoothVertexNormals.begin(); iter != smoothVertexNormals.end();
				 iter++) {
			normalizeVector(iter->second);
		}

	}  // computeSmoothingNormals

	Model create_model_from_drawobjects(const tinyobj::attrib_t &attrib,
		const std::vector<tinyobj::shape_t> &shapes,
		const std::vector<tinyobj::material_t> &materials,
		std::map<std::string, uint> *textures,
		Model &model,
		const std::string &model_name)
	{
		float bmin[3], bmax[3];
		std::vector<DrawObject> drawobjects;
		bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
		bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

		// Create DrawObject for each shape
		for (size_t s = 0; s < shapes.size(); s++)
		{
			DrawObject o;
			std::vector<float> buffer;  // pos(3float), normal(3float), color(3float), texcoord(2float)

			// Check for smoothing group and compute smoothing normals
			std::map<int, vec3> smoothVertexNormals;
			if (hasSmoothingGroup(shapes[s]) > 0)
			{
				// std::cout << "  Compute smoothingNormal for shape [" << s << "]" << std::endl;
				computeSmoothingNormals(attrib, shapes[s], smoothVertexNormals);
			}
			for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
			{
				tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
				tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
				tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

				int current_material_id = shapes[s].mesh.material_ids[f];

				if ((current_material_id < 0) ||
						(current_material_id >= static_cast<int>(materials.size())))
				{
					// Invaid material ID. Use default material.
					current_material_id =
							materials.size() -
							1;  // Default material is added to the last item in `materials`.
				}
				// if (current_material_id >= materials.size()) {
				//    std::cerr << "Invalid material index: " << current_material_id <<
				//    std::endl;
				//}
				//
				// Not very important
				{
					float diffuse[3];
					for (size_t i = 0; i < 3; i++)
					{
						diffuse[i] = materials[current_material_id].diffuse[i];
					}
					float tc[3][2];
					if (attrib.texcoords.size() > 0)
					{
						if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
								(idx2.texcoord_index < 0)) {
							// face does not contain valid uv index.
							tc[0][0] = 0.0f;
							tc[0][1] = 0.0f;
							tc[1][0] = 0.0f;
							tc[1][1] = 0.0f;
							tc[2][0] = 0.0f;
							tc[2][1] = 0.0f;
						} else {
							assert(attrib.texcoords.size() >
										 size_t(2 * idx0.texcoord_index + 1));
							assert(attrib.texcoords.size() >
										 size_t(2 * idx1.texcoord_index + 1));
							assert(attrib.texcoords.size() >
										 size_t(2 * idx2.texcoord_index + 1));

							// Flip Y coord.
							tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
							tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
							tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
							tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
							tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
							tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
						}
					}
					else
					{
						tc[0][0] = 0.0f;
						tc[0][1] = 0.0f;
						tc[1][0] = 0.0f;
						tc[1][1] = 0.0f;
						tc[2][0] = 0.0f;
						tc[2][1] = 0.0f;
					}

					float v[3][3];
					for (int k = 0; k < 3; k++)
					{
						int f0 = idx0.vertex_index;
						int f1 = idx1.vertex_index;
						int f2 = idx2.vertex_index;
						assert(f0 >= 0);
						assert(f1 >= 0);
						assert(f2 >= 0);

						v[0][k] = attrib.vertices[3 * f0 + k];
						v[1][k] = attrib.vertices[3 * f1 + k];
						v[2][k] = attrib.vertices[3 * f2 + k];
						bmin[k] = std::min(v[0][k], bmin[k]);
						bmin[k] = std::min(v[1][k], bmin[k]);
						bmin[k] = std::min(v[2][k], bmin[k]);
						bmax[k] = std::max(v[0][k], bmax[k]);
						bmax[k] = std::max(v[1][k], bmax[k]);
						bmax[k] = std::max(v[2][k], bmax[k]);
					}

					float n[3][3];
					{
						bool invalid_normal_index = false;
						if (attrib.normals.size() > 0)
						{
							int nf0 = idx0.normal_index;
							int nf1 = idx1.normal_index;
							int nf2 = idx2.normal_index;

							if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0))
							{
								// normal index is missing from this face.
								invalid_normal_index = true;
							}
							else
							{
								for (int k = 0; k < 3; k++)
								{
									assert(size_t(3 * nf0 + k) < attrib.normals.size());
									assert(size_t(3 * nf1 + k) < attrib.normals.size());
									assert(size_t(3 * nf2 + k) < attrib.normals.size());
									n[0][k] = attrib.normals[3 * nf0 + k];
									n[1][k] = attrib.normals[3 * nf1 + k];
									n[2][k] = attrib.normals[3 * nf2 + k];
								}
							}
						}
						else
						{
							invalid_normal_index = true;
						}

						if (invalid_normal_index && !smoothVertexNormals.empty())
						{
							// Use smoothing normals
							int f0 = idx0.vertex_index;
							int f1 = idx1.vertex_index;
							int f2 = idx2.vertex_index;

							if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
								n[0][0] = smoothVertexNormals[f0].v[0];
								n[0][1] = smoothVertexNormals[f0].v[1];
								n[0][2] = smoothVertexNormals[f0].v[2];

								n[1][0] = smoothVertexNormals[f1].v[0];
								n[1][1] = smoothVertexNormals[f1].v[1];
								n[1][2] = smoothVertexNormals[f1].v[2];

								n[2][0] = smoothVertexNormals[f2].v[0];
								n[2][1] = smoothVertexNormals[f2].v[1];
								n[2][2] = smoothVertexNormals[f2].v[2];

								invalid_normal_index = false;
							}
						}

						if (invalid_normal_index)
						{
							// compute geometric normal
							CalcNormal(n[0], v[0], v[1], v[2]);
							n[1][0] = n[0][0];
							n[1][1] = n[0][1];
							n[1][2] = n[0][2];
							n[2][0] = n[0][0];
							n[2][1] = n[0][1];
							n[2][2] = n[0][2];
						}
					}

					for (int k = 0; k < 3; k++)
					{
						buffer.push_back(v[k][0]);
						buffer.push_back(v[k][1]);
						buffer.push_back(v[k][2]);
						buffer.push_back(n[k][0]);
						buffer.push_back(n[k][1]);
						buffer.push_back(n[k][2]);
						// Combine normal and diffuse to get color.
						float normal_factor = 0.2;
						float diffuse_factor = 1 - normal_factor;
						float c[3] = {n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
													n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
													n[k][2] * normal_factor + diffuse[2] * diffuse_factor};
						float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
						if (len2 > 0.0f) {
							float len = sqrtf(len2);

							c[0] /= len;
							c[1] /= len;
							c[2] /= len;
						}
						buffer.push_back(diffuse[0]);//1.0);//c[0] * 0.5 + 0.5);
						buffer.push_back(diffuse[1]);//1.0);//c[1] * 0.5 + 0.5);
						buffer.push_back(diffuse[2]);//1.0);//c[2] * 0.5 + 0.5);

						buffer.push_back(tc[k][0]);
						buffer.push_back(tc[k][1]);
					}
				} // end not very important code
			}
			o.vbo = 0;
			o.vao = 0;
			o.numTriangles = 0;

			// std::cout << "hej0" << '\n';
			// OpenGL viewer does not support texturing with per-face material.
			if (shapes[s].mesh.material_ids.size() > 0 && shapes[s].mesh.material_ids.size() > s)
			{
				o.material_id = shapes[s].mesh.material_ids[0];  // use the material ID of the first face.
				
			}
			else
			{
				o.material_id = materials.size() - 1;  // = ID for default material.
			}
			
			// std::cout << "materials.size()=" << materials.size() << '\n';
			// std::cout << "o.material_id=" << o.material_id << '\n';
			o.texture_name = materials[o.material_id].diffuse_texname;
			o.texture_id = (*textures)[o.texture_name];
			
			if (buffer.size() > 0)
			{
				glGenVertexArrays(1, &o.vao);
				glGenBuffers(1, &o.vbo);

				glBindVertexArray(o.vao);
				glBindBuffer(GL_ARRAY_BUFFER, o.vbo);
				glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0), GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);
				GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float)*3));
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float)*6));
				glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float)*9));

				o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) /
												 3;  // 3:vtx, 3:normal, 3:col, 2:texcoord

				// printf("shape[%d] # of triangles = %d\n", static_cast<int>(s),
				// 			 o.numTriangles);
			}

			drawobjects.push_back(o);
		}
		return std::move(Model(model_name, std::move(drawobjects), bmin, bmax));
	} // create_model_from_drawobject
} // unnamed implementation-depended functions not to be exposed outwards
