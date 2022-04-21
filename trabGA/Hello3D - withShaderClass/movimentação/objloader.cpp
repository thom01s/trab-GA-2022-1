#pragma once

#include "objloader.h"

int meshSize;

int getMeshSize() {
	return meshSize;
}

vector <string> tokenize(string buffer, string delim) {
	vector <string> tokens;
	string strbuffer(buffer);
	string token;
	int pos;

	do {
		pos = strbuffer.find(delim);
		if (pos > 0) {
			token = strbuffer.substr(0, pos);
			strbuffer = strbuffer.substr(pos + delim.size());
			tokens.push_back(token);
		}
		else tokens.push_back(strbuffer);
	} while (pos > 0);

	return tokens;
}

int setupMesh(string path)
{
	vector <glm::vec3> vertices, normals;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> vertIndices, normIndices, texIndices; //indices s�o ints, mas s�o 3

	ifstream inputFile;
	inputFile.open(path);
	if (inputFile.is_open())
	{
		char line[100];
		string sline;
		int cont = 1;
		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			if (!inputFile.eof())
			{
				//cout << cont << " --> *" << line << "*" << endl;
				cont++;
				sline = line;
				string word;
				istringstream ssline(line);
				ssline >> word;
				if (word == "v" || word == "vn" || word == "vt" || word == "f")
				{
					glm::vec3 v3;
					if (word == "v")
					{
						ssline >> v3.x >> v3.y >> v3.z;
						vertices.push_back(v3);
					}
					if (word == "vn")
					{
						ssline >> v3.x >> v3.y >> v3.z;
						normals.push_back(v3);
					}
					if (word == "vt")
					{
						glm::vec2 v2;
						ssline >> v2.x >> v2.y;
						texCoords.push_back(v2);
					}
					if (word == "f")
					{
						string i1, i2, i3;
						glm::vec3 iv, ivn, ivt;
						ssline >> i1 >> i2 >> i3;
						vector <string> tokens;

						//Pegando os primeiros �ndices (considerando que tem todos)
						tokens = tokenize(i1, "/");
						iv[0] = atof(tokens[0].c_str());
						ivt[0] = atof(tokens[1].c_str());
						ivn[0] = atof(tokens[2].c_str());

						//Pegando os segundos �ndices (considerando que tem todos)
						tokens = tokenize(i2, "/");
						iv[1] = atof(tokens[0].c_str());
						ivt[1] = atof(tokens[1].c_str());
						ivn[1] = atof(tokens[2].c_str());

						//Pegando os terceiros �ndices (considerando que tem todos)
						tokens = tokenize(i3, "/");
						iv[2] = atof(tokens[0].c_str());
						ivt[2] = atof(tokens[1].c_str());
						ivn[2] = atof(tokens[2].c_str());

						vertIndices.push_back(iv);
						normIndices.push_back(ivn);
						texIndices.push_back(ivt);
					}

				}
				else
				{
					//ignorando o resto por enquanto
				}
			}//if (!inputFile.eof())
			else
			{
				cout << "Foi at� o fim!" << endl;
			}
		}
		inputFile.close();

		vector <float> geometryBuffer;

		meshSize = vertIndices.size() * 3;

		for (int i = 0; i < vertIndices.size(); i++)
		{
			for (int j = 0; j < 3; j++) //triangulo
			{
				//recupera x, y, z (posicao)
				float x = vertices[(int)vertIndices[i][j] - 1].x;
				float y = vertices[(int)vertIndices[i][j] - 1].y;
				float z = vertices[(int)vertIndices[i][j] - 1].z;
				geometryBuffer.push_back(x);
				geometryBuffer.push_back(y);
				geometryBuffer.push_back(z);

				//recupera nx, ny, nz (normal)
				float nx = normals[(int)normIndices[i][j] - 1].x;
				float ny = normals[(int)normIndices[i][j] - 1].y;
				float nz = normals[(int)normIndices[i][j] - 1].z;
				geometryBuffer.push_back(nx);
				geometryBuffer.push_back(ny);
				geometryBuffer.push_back(nz);

				//recupera s, t (coord textura)
				/*float s = texCoords[(int)vertIndices[i][j]].x;
				float t = texCoords[(int)vertIndices[i][j]].y;
				geometryBuffer.push_back(s);
				geometryBuffer.push_back(t);*/
			}
		}
		GLuint VBO, VAO;

		//Gera��o do identificador do VBO
		glGenBuffers(1, &VBO);

		//Faz a conex�o (vincula) do buffer como um buffer de array
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//Envia os dados do array de floats para o buffer da OpenGl
		glBufferData(GL_ARRAY_BUFFER, geometryBuffer.size() * sizeof(GLfloat), geometryBuffer.data(), GL_STATIC_DRAW);

		//Gera��o do identificador do VAO (Vertex Array Object)
		glGenVertexArrays(1, &VAO);

		// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
		// e os ponteiros para os atributos 
		glBindVertexArray(VAO);

		//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
		// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
		// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
		// Tipo do dado
		// Se est� normalizado (entre zero e um)
		// Tamanho em bytes 
		// Deslocamento a partir do byte zero 

		//Atributo posi��o (x, y, z)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		//Atributo normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
		// atualmente vinculado - para que depois possamos desvincular com seguran�a
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
		glBindVertexArray(0);

		return VAO;
	}
	else
	{
		cout << "Erro ao ler o arquivo!" << endl;
		return -1;
	}
	return 0;
}