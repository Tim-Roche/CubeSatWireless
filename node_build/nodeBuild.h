// nodeBuild.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


	class node
	{
	public:
		// CONSTRUCTOR
		node(
			int init_UUID,
			int init_node_type = 1
		)
		{
			UUID = init_UUID;
			node_type = init_node_type;
		}
		
		int getUUID();
		int getNodeType();

	private:
		int UUID;
		int node_type;
	};
