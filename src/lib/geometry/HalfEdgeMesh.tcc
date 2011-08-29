/*
 * HalfEdgeMesh.cpp
 *
 *  Created on: 13.11.2008
 *      Author: Thomas Wiemann
 */

namespace lssr
{



template<typename VertexT, typename NormalT>
HalfEdgeMesh<VertexT, NormalT>::HalfEdgeMesh()
{
	m_globalIndex = 0;
	m_colorRegions = false;
}


template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::addVertex(VertexT v)
{
	// Create new HalfEdgeVertex and increase vertex counter
	m_vertices.push_back(new HalfEdgeVertex<VertexT, NormalT>(v));
	m_globalIndex++;
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::deleteVertex(HVertex* v)
{
	// Delete HalfEdgeVertex and decrease vertex counter
	typename vector<HVertex*>::iterator it = m_vertices.begin();
	while(*it != v) it++;
	m_vertices.erase(it);
	m_globalIndex--;
	delete v;
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::addNormal(NormalT n)
{
	// Is a vertex exists at globalIndex, save normal
	assert(m_globalIndex == m_vertices.size());
	m_vertices[m_globalIndex - 1]->m_normal = n;
}

template<typename VertexT, typename NormalT>
HalfEdge<HalfEdgeVertex<VertexT, NormalT>, HalfEdgeFace<VertexT, NormalT> >* HalfEdgeMesh<VertexT, NormalT>::halfEdgeToVertex(HVertex *v, HVertex* next)
{
	HEdge* edge = 0;
	HEdge* cur = 0;

	typename vector<HEdge*>::iterator it;

	for(it = v->in.begin(); it != v->in.end(); it++)
	{
		// Check all incoming edges, if start and end vertex
		// are the same. If they are, save this edge.
		cur = *it;
		if(cur->end == v && cur->start == next)
		{
			edge = cur;
		}

	}

	return edge;
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::addTriangle(uint a, uint b, uint c)
{
	// Create a new face
	HFace* face = new HFace;

	// Create a list of HalfEdges that will be connected
	// with this here. Here we need only to alloc space for
	// three pointers, allocation and linking will be done
	// later.
	HEdge* edges[3];
	edges[0] = edges[1] = edges[2] = 0;

	// Traverse face triangles
	for(int k = 0; k < 3; k++)
	{
		// Pointer to start and end vertex of an edge
		HVertex* current;
		HVertex* next;

		// Map k values to parameters
		switch(k)
		{
		case 0:
			current = m_vertices[a];
			next 	= m_vertices[b];
			break;
		case 1:
			current = m_vertices[b];
			next 	= m_vertices[c];
			break;
		case 2:
			current = m_vertices[c];
			next 	= m_vertices[a];
			break;
		}

		// Try to find an pair edges of an existing face,
		// that points to the current vertex. If such an
		// edge exists, the pair-edge of this edge is the
		// one we need. Update link. If no edge is found,
		// create a new one.
		HEdge* edgeToVertex = halfEdgeToVertex(current, next);

		// If a fitting edge was found, save the pair edge
		// and let it point the the new face
		if(edgeToVertex != 0)
		{
			edges[k] = edgeToVertex->pair;
			edges[k]->face = face;
		}
		else
		{
			// Create new edge and pair
			HEdge* edge = new HEdge;
			edge->face = face;
			edge->start = current;
			edge->end = next;

			HEdge* pair = new HEdge;
			pair->start = next;
			pair->end = current;
			pair->face = 0;

			// Link Half edges
			edge->pair = pair;
			pair->pair = edge;

			// Save outgoing edge
			current->out.push_back(edge);
			next->in.push_back(edge);

			// Save incoming edges
			current->in.push_back(pair);
			next->out.push_back(pair);

			// Save pointer to new edge
			edges[k] = edge;
		}
	}
	
    for(int k = 0; k < 3; k++)
	{
		edges[k]->next = edges[(k+1) % 3];
	}
	//cout << ":: " << face->index[0] << " " << face->index[1] << " " << face->index[2] << endl;

	face->m_edge = edges[0];
	face->calc_normal();
	m_faces.push_back(face);
	face->m_face_index = m_faces.size();
	//face->m_index[0] = a;
	//face->m_index[1] = b;
	//face->m_index[2] = c;

//	if(a == 0) {
//		last_normal = face->normal;
//	} else {
//		if(last_normal * face->normal < 0){
//			face->normal = face->normal * -1;
//		}
//	}

}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::addFace(HVertex* v1, HVertex* v2, HVertex* v3)
{
	HFace* f = new HFace;

	HEdge* v1v2 = 0;
	HEdge* v2v3 = 0;
	HEdge* v3v1 = 0;

	HEdge* current = 0;

	//check if edge exists between v1, v2 if not add a new one
	if((current = halfEdgeToVertex(v1, v2)) == 0)
	{
		v1v2 = new HEdge;
		HEdge* v2v1 = new HEdge;
		typename vector<HEdge*>::iterator it;

		v1v2->start = v1;
		v1v2->end = v2;
		v1->out.push_back(v1v2);
		v2->in.push_back(v1v2);

		v2v1->start = v2;
		v2v1->end = v1;
		v1->in.push_back(v2v1);
		v2->out.push_back(v2v1);

		v1v2->pair = v2v1;
		v2v1->pair = v1v2;
	}
	else
	{
		if(current->face == 0)
		{
			v1v2 = current;
		}
		else
		{
		    v1v2 = current->pair;
		}
	}

	//check if edge exists between v2, v3 if not add a new one
	if((current = halfEdgeToVertex(v2, v3)) == 0)
	{
		v2v3 = new HEdge;
		HEdge* v3v2 = new HEdge;

		v2v3->start = v2;
		v2v3->end = v3;
		v2->out.push_back(v2v3);
		v3->in.push_back(v2v3);

		v3v2->start = v3;
		v3v2->end = v2;
		v2->in.push_back(v3v2);
		v3->out.push_back(v3v2);

		v2v3->pair = v3v2;
		v3v2->pair = v2v3;
	}
	else
	{
		if(current->face == 0)
			v2v3 = current;
		else v2v3 = current->pair;
	}

	//check if edge exists between v3, v1 if not add a new one
	if((current = halfEdgeToVertex(v3, v1)) == 0)
	{
		v3v1 = new HEdge;
		HEdge* v1v3 = new HEdge;

		v3v1->start = v3;
		v3v1->end = v1;
		v3->out.push_back(v3v1);
		v1->in.push_back(v3v1);

		v1v3->start = v1;
		v1v3->end = v3;
		v3->in.push_back(v1v3);
		v1->out.push_back(v1v3);

		v3v1->pair = v1v3;
		v1v3->pair = v3v1;
	}
	else
	{
		if(current->face == 0)
		{
			v3v1 = current;
		}
		else
		{
		    v3v1 = current->pair;
		}
	}

	// set next pointers
	typename vector<HEdge*>::iterator it;
	it = v1v2->end->out.begin();
	while(it != v1v2->end->out.end() && *it != v2v3) it++;
	if(it != v1v2->end->out.end())
		v1v2->next = v2v3;
	else
		v1v2->next = v2v3->pair;

	it = v1v2->next->end->out.begin();
	while(it != v1v2->next->end->out.end() && *it != v3v1) it++;

	if(it != v1v2->next->end->out.end())
	{
		v1v2->next->next = v3v1;
	}
	else
	{
		v1v2->next->next = v3v1->pair;
	}

	v1v2->next->next->next = v1v2;

	//set face->m_edge
	f->m_edge = v1v2;

	//set face pointers
	current = v1v2;
	for(int k = 0; k<3; k++,current = current->next)
		current->face = f;

	f->calc_normal();
	m_faces.push_back(f);
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::deleteFace(HFace* f)
{
	//save references to edges and vertices
	HEdge* startEdge = (*f)[0];
	HEdge* nextEdge  = (*f)[1];
	HEdge* lastEdge  = (*f)[2];
	HVertex* p1 = (*f)(0);
	HVertex* p2 = (*f)(1);
	HVertex* p3 = (*f)(2);

	startEdge->face = 0;
	nextEdge->face = 0;
	lastEdge->face = 0;

	typename vector<HEdge*>::iterator it;

	if(startEdge->pair->face == 0)
	{
		//delete edge and pair
		deleteEdge(startEdge);

		if(p1->out.size()==0) deleteVertex(p1);
		if(p3->out.size()==0) deleteVertex(p3);
	}

	if(nextEdge->pair->face == 0)
	{
		//delete edge and pair
		deleteEdge(nextEdge);

		if(p1->out.size()==0) deleteVertex(p1);
		if(p2->out.size()==0) deleteVertex(p2);
	}

	if(lastEdge->pair->face == 0)
	{
		//delete edge and pair
		deleteEdge(lastEdge);

		if(p3->out.size()==0) deleteVertex(p3);
		if(p2->out.size()==0) deleteVertex(p2);
	}

	//delete face
	typename vector<HalfEdgeFace<VertexT, NormalT>*>::iterator face_iter = m_faces.begin();
	while(*face_iter != f) face_iter++;
	m_faces.erase(face_iter);
	delete f;
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::deleteEdge(HEdge* edge, bool deletePair)
{
	typename vector<HEdge*>::iterator it;

	//delete references from start point to outgoing edge
	it = edge->start->out.begin();
	while(*it != edge) it++;
	edge->start->out.erase(it);

	//delete references from end point to incoming edge
	it = edge->end->in.begin();
	while(*it != edge) it++;
	edge->end->in.erase(it);

	if(deletePair)
	{
		//delete references from start point to outgoing edge
		it = edge->pair->start->out.begin();
		while(*it != edge->pair) it++;
		edge->pair->start->out.erase(it);

		//delete references from end point to incoming edge
		it = edge->pair->end->in.begin();
		while(*it != edge->pair) it++;
		edge->pair->end->in.erase(it);

		delete edge->pair;
	}
	delete edge;
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::collapseEdge(HEdge* edge)
{
	// Save start and end vertex
	HVertex* p1 = edge->start;
	HVertex* p2 = edge->end;

	// Move p1 to the center between p1 and p2 (recycle p1)
	p1->m_position = (p1->m_position + p2->m_position)*0.5;

	//Delete redundant edges
	typename vector<HEdge*>::iterator it;

	if (edge->face != 0)
	{
		edge->next->next->pair->pair = edge->next->pair;
		edge->next->pair->pair = edge->next->next->pair;
		deleteEdge(edge->next->next, false);
		deleteEdge(edge->next, false);
	}

	if (edge->pair->face != 0)
	{
		edge->pair->next->next->pair->pair = edge->pair->next->pair;
		edge->pair->next->pair->pair = edge->pair->next->next->pair;
		deleteEdge(edge->pair->next->next, false);
		deleteEdge(edge->pair->next, false);
	}

	// Delete faces
	typename	vector<HalfEdgeFace<VertexT, NormalT>*>::iterator face_iter;
	if(edge->pair->face != 0)
	{
		face_iter = m_faces.begin();
		while(*face_iter != edge->pair->face) face_iter++;
		m_faces.erase(face_iter);
		delete edge->pair->face;
	}

	if(edge->face != 0)
	{
		face_iter = m_faces.begin();
		while(*face_iter != edge->face) face_iter++;
		m_faces.erase(face_iter);
		delete edge->face;
	}

	//Delete edge and its' pair
	deleteEdge(edge);

	//Update incoming and outgoing edges of p1
	it = p2->out.begin();
	while(it != p2->out.end())
	{
		(*it)->start = p1;
		p1->out.push_back(*it);
		it++;
	}
	it = p2->in.begin();

	while(it != p2->in.end())
	{
		(*it)->end = p1;
		p1->in.push_back(*it);
		it++;
	}

	//Delete p2
	deleteVertex(p2);
}


template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::flipEdge(HFace* f1, HFace* f2)
{
	HEdge* commonEdge = 0;
	HEdge* current = f1->m_edge;

	//search the common edge between the two faces
	for(int k = 0; k < 3; k++)
	{
		if (current->pair->face == f2) commonEdge = current;
		current = current->next;
	}

	//return if f1 and f2 are not adjacent in the grid
	if(commonEdge == 0)
	{
		return;
	}

	//flip the common edge
	this->flipEdge(commonEdge);
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::flipEdge(HEdge* edge)
{
	if (edge->pair->face != 0 && edge->face != 0)
	{
		HVertex* newEdgeStart = edge->next->end;
		HVertex* newEdgeEnd = edge->pair->next->end;

		//update next pointers
		edge->next->next->next = edge->pair->next;
		edge->pair->next->next->next = edge->next;

		//create the new edge
		HEdge* newEdge = new HEdge();
		newEdge->start = newEdgeStart;
		newEdge->end = newEdgeEnd;
		newEdge->pair = 0;
		newEdge->next = edge->pair->next->next;
		newEdge->face = edge->pair->next->next->face;
		newEdge->start->out.push_back(newEdge);
		newEdge->end->in.push_back(newEdge);

		HEdge* newPair = new HEdge();
		newPair->start = newEdgeEnd;
		newPair->end = newEdgeStart;
		newPair->pair = newEdge;
		newPair->next = edge->next->next;
		newPair->face = edge->next->next->face;
		newPair->start->out.push_back(newPair);
		newPair->end->in.push_back(newPair);

		newEdge->pair = newPair;

		//update face->edge pointers
		newEdge->face->m_edge = newEdge;
		newPair->face->m_edge = newPair;

		//update next pointers
		edge->next->next = newEdge;
		edge->pair->next->next = newPair;

		//update edge->face pointers
		newEdge->next->face = newEdge->face;
		newEdge->next->next->face = newEdge->face;
		newPair->next->face = newPair->face;
		newPair->next->next->face = newPair->face;

		//recalculate face normals
		newEdge->face->calc_normal();
		newPair->face->calc_normal();

		//delete the old edge
		deleteEdge(edge);
	}
}

template<typename VertexT, typename NormalT>
int HalfEdgeMesh<VertexT, NormalT>::regionGrowing(HFace* start_face, Region<VertexT, NormalT>* region)
{
    //Mark face as used
    start_face->m_used = true;

    //Add face to region
    region->addFace(start_face);

    int neighbor_cnt = 0;

    //Get the unmarked neighbor faces and start the recursion
    for(int k=0; k<3; k++)
    {
        if((*start_face)[k]->pair->face != 0 && (*start_face)[k]->pair->face->m_used == false)
            ++neighbor_cnt += regionGrowing((*start_face)[k]->pair->face, region);
    }

    return neighbor_cnt;
}

template<typename VertexT, typename NormalT>
int HalfEdgeMesh<VertexT, NormalT>::regionGrowing(HFace* start_face, NormalT &normal, float &angle, Region<VertexT, NormalT>* region)
{
    //Mark face as used
    start_face->m_used = true;

    //Add face to region
    region->addFace(start_face);

    int neighbor_cnt = 0;

    //Get the unmarked neighbor faces and start the recursion
    for(int k=0; k<3; k++)
    {
        if((*start_face)[k]->pair->face != 0 && (*start_face)[k]->pair->face->m_used == false
                && fabs((*start_face)[k]->pair->face->getFaceNormal() * normal) > angle )
            ++neighbor_cnt += regionGrowing((*start_face)[k]->pair->face, normal, angle, region);
    }

    return neighbor_cnt;
}


template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::optimizePlanes(
        int iterations,
        float angle,
        int min_region_size,
        int small_region_size
        )
{
    cout << timestamp << "Starting plane optimization with threshold " << angle << endl;

   // Magic numbers
    int default_region_threshold = (int)10*log(m_faces.size());

	// Regions that will be deleted due to size
	vector<Region<VertexT, NormalT>*> smallRegions;

	int region_size = 0;
	m_regions.clear();
	int region_number = 0;

	for(int j = 0; j < iterations; j++)
	{
		cout << timestamp << "Optimizing planes. " <<  j << "th iteration." << endl;

		// Reset all used variables
		for(int i=0; i < m_faces.size(); i++)
		{
			m_faces[i]->m_used = false;
		}

		// Find all regions by regionGrowing with normal criteria
		for(int i=0; i < m_faces.size(); i++)
		{
			if(m_faces[i]->m_used == false)
			{
				NormalT n = m_faces[i]->getFaceNormal();

				Region<VertexT, NormalT>* region = new Region<VertexT, NormalT>(region_number);
				region_size = regionGrowing(m_faces[i], n, angle, region) + 1;

				// Fit big regions into the regression plane
				if(region_size > max(min_region_size, default_region_threshold))
				{
					region->regressionPlane();
				}

				if(j == iterations-1)
				{
					// Save too small regions with size smaller than small_region_size
					if (region_size < small_region_size)
					{
						smallRegions.push_back(region);
					}
					else
					{
					    // Save pointer to the region
						m_regions.push_back(region);
						region_number++;
					}
				}
				else
				{
					delete region;
				}
			}
		}
	}

	// Delete too small regions
	if(small_region_size)
	{
	    string msg = timestamp.getElapsedTime() + "Deleting small regions.";
	    ProgressBar progress(smallRegions.size(), msg);
	    for(int i=0; i< smallRegions.size(); i++)
	    {
	        deleteRegion(smallRegions[i]);
	        ++progress;
	    }
	}
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::deleteRegion(Region<VertexT, NormalT>* region)
{
    for(int i=0; i<region->m_faces.size(); i++)
        deleteFace(region->m_faces[i]);
    delete region;
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::removeDanglingArtifacts(int threshold)
{
    vector<Region<VertexT, NormalT>*> todelete;

    for(int i=0; i<m_faces.size(); i++)
    {
        if(m_faces[i]->m_used == false)
        {
        	Region<VertexT, NormalT>* region = new Region<VertexT, NormalT>(-1);
            int region_size = regionGrowing(m_faces[i], region) + 1;
            if(region_size <= threshold)
                todelete.push_back(region);
            else
            	delete region;
        }
    }

    for(int i=0; i<todelete.size(); i++ )
        deleteRegion(todelete[i]);

    //reset all used variables
    for(int i=0; i<m_faces.size(); i++)
        m_faces[i]->m_used = false;
}


template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::fillHoles(int max_size)
{
    //walk through all edges and start hole finding
    //when pair has no face and a regression plane was applied
    for(int i=0; i < m_faces.size(); i++)
        for(int k=0; k<3; k++)
            if((*m_faces[i])[k]->pair->face == 0 && m_regions[m_faces[i]->m_region]->m_inPlane)
            {
                //needed for contour tracking
                vector<HalfEdgeVertex<VertexT, NormalT>*> contour;
                HEdge* next = 0;
                HEdge* current = (*m_faces[i])[k]->pair;

                //needed to check if only two different regions are considered
                HFace* first_region = m_faces[i];
                HFace* second_region = 0;
                int number_of_regions = 1;

                //while the contour is not closed
                while(current != 0)
                {
                    next = 0;
                    contour.push_back(current->end);
                    current->used = true;
                    typename vector<HEdge*>::iterator it = current->end->out.begin();
                    while(it != current->end->out.end())
                    {
                        //found a new possible edge to trace
                        if ((*it)->used == false && (*it)->face == 0)
                        {
                            next = *it;
                            //next is part of a new region
                            if(next->pair->face->m_region != first_region->m_region)
                                if (second_region != 0 && next->pair->face->m_region != second_region->m_region)
                                    number_of_regions = -1;
                                else
                                {
                                    //if two regions were found
                                    second_region = next->pair->face;
                                    number_of_regions = 2;
                                }
                        }
                        it++;
                    }

                    current = next;
                }

                if(number_of_regions == 1)
                {
                    //Is the contour of acceptable size and closed?
                    if(2 < contour.size() && contour.size() < max_size && halfEdgeToVertex(contour[contour.size()-1], contour[0]) != 0)
                    {
                        //Simulate a face by setting the face pointer of the edge next to the hole
                        for(int p=0; p<contour.size(); p++)
                        {
                            HEdge* edge = halfEdgeToVertex(contour[p], contour[(p+1) % contour.size()]);
                            if(edge->face == 0)
                                edge->face = edge->pair->face;
                            else
                                edge->pair->face = edge->face;
                        }
                        //Just for testing purposes
                        HalfEdgeVertex<VertexT, NormalT> newPoint;
                        for (int c = 0; c<contour.size(); c++)
                            newPoint.m_position += contour[c]->m_position;
                        newPoint.m_position /= contour.size();

                        for (int c = 0; c<contour.size(); c++)
                            contour[c]->m_position = newPoint.m_position;
                    }
                }
                else if (number_of_regions == 2)
                {
                    //Is the contour of acceptable size and closed?
                    if(2 < contour.size() && contour.size() < max_size && halfEdgeToVertex(contour[contour.size()-1], contour[0]) != 0)
                    {
                        //Simulate a face by setting the face pointer of the edge next to the hole
                        //Especially: Simulate that there exists a pair edge of the second region
                        //This is necessary for optimizePlaneintersections()
                        for(int p=0; p<contour.size(); p++)
                        {
                            HEdge* edge = halfEdgeToVertex(contour[p], contour[(p+1) % contour.size()]);
                            if(edge->face == 0)
                            {
                                if(edge->pair->face->m_region == first_region->m_region)
                                    edge->face = second_region;
                                else
                                    edge->face = first_region;
                            }
                            else
                            {
                                if(edge->face->m_region == first_region->m_region)
                                    edge->pair->face= second_region;
                                else
                                    edge->pair->face = first_region;
                            }
                        }
                    }
                }
            }
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::dragOntoIntersection(Region<VertexT, NormalT>* plane, int neighbor_region, VertexT& x, VertexT& direction)
{
	for (int i = 0; i<plane->m_faces.size(); i++)
	{
	    for(int k=0; k<=2; k++)
	    {
	        if((*(plane->m_faces[i]))[k]->pair->face != 0 && (*(plane->m_faces[i]))[k]->pair->face->m_region == neighbor_region)
	        {
	        	(*(plane->m_faces[i]))[k]->start->m_position = x + direction * (((((*(plane->m_faces[i]))[k]->start->m_position)-x) * direction) / (direction.length() * direction.length()));
	        	(*(plane->m_faces[i]))[k]->end->m_position   = x + direction * (((((*(plane->m_faces[i]))[k]->end->m_position  )-x) * direction) / (direction.length() * direction.length()));
	        }
	    }
	}
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::optimizePlaneIntersections()
{
    for (int i = 0; i<m_regions.size(); i++)
    {
        if (m_regions[i]->m_inPlane)
            for(int j = i+1; j<m_regions.size(); j++)
                if(m_regions[j]->m_inPlane)
                {
                    //calculate intersection between plane i and j

                	NormalT n_i = m_regions[i]->getNormal();
                	NormalT n_j = m_regions[j]->getNormal();

                    //don't improve almost parallel regions - they won't cross in a reasonable distance
                    if (fabs(n_i*n_j) < 0.9)
                    {

                        float d_i = n_i * (*(m_regions[i]->m_faces[0]))(0)->m_position;
                        float d_j = n_j * (*(m_regions[j]->m_faces[0]))(0)->m_position;
                        float n_i1 = n_i[0];
                        float n_i2 = n_i[1];
                        float n_j1 = n_j[0];
                        float n_j2 = n_j[1];

                        float x1 = (d_i/n_i1 - ((n_i2*d_j)/(n_j2*n_i1)))/(1-((n_i2*n_j1)/(n_j2*n_i1)));
                        float x2 = (d_j-n_j1*x1)/n_j2;
                        float x3 = 0;
                        VertexT x (x1, x2, x3);

                        VertexT direction = n_i.cross(n_j);

                        //drag all points at the border between planes i and j onto the intersection
                        dragOntoIntersection(m_regions[i], m_regions[j]->m_region_number, x, direction);
                        dragOntoIntersection(m_regions[j], m_regions[i]->m_region_number, x, direction);
                    }
                }
    }
}

template<typename VertexT, typename NormalT>
vector<stack<HalfEdgeVertex<VertexT, NormalT>* > > HalfEdgeMesh<VertexT, NormalT>::findAllContours(float epsilon)
{
    vector<stack<HalfEdgeVertex<VertexT, NormalT>* > > contours;
    for (int i = 0; i< m_regions.size(); i++)
    {
    	if(m_regions[i]->m_inPlane){
    		vector<stack<HalfEdgeVertex<VertexT, NormalT>* > > current_contours = m_regions[i]->getContours(epsilon);
    		contours.insert(contours.end(), current_contours.begin(), current_contours.end());
    	}
    }
    return  contours;
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::tester()
{
//	removeDanglingArtifacts(500);
//	optimizePlanes(3,0.85,50,7);
//	fillHoles(35);
//	optimizePlaneIntersections();
//	m_colorRegions = true;

    //Reset all used variables
    for(int i=0; i<m_faces.size(); i++)
        for(int k=0; k<3; k++)
            (*m_faces[i])[k]->used=false;

    vector<stack<HalfEdgeVertex<VertexT, NormalT>* > > contours = findAllContours(0.1);
    fstream filestr;
    filestr.open ("contours.pts", fstream::out);
    filestr<<"#X Y Z"<<endl;
    for (int i = 0; i<contours.size(); i++)
    {
        stack<HalfEdgeVertex<VertexT, NormalT>* > contour = contours[i];

        HalfEdgeVertex<VertexT, NormalT> first = *(contour.top());

        while (!contour.empty())
        {
            filestr << contour.top()->m_position[0] << " " << contour.top()->m_position[1] << " " << contour.top()->m_position[2] << endl;
            contour.pop();
        }

        filestr << first.m_position[0] << " " << first.m_position[1] << " " << first.m_position[2] << endl;

        filestr<<endl<<endl;

    }
    filestr.close();

//  for(int i=0; i<m_faces.size(); i++)
//      m_faces[i]->m_region=0;
}

template<typename VertexT, typename NormalT>
void HalfEdgeMesh<VertexT, NormalT>::finalize()
{
    cout << timestamp << "Finalizing mesh." << endl;
	cout << timestamp << "Number of vertices: " << (uint32_t) m_vertices.size() << endl;
	cout << timestamp << "Number of faces: " << (uint32_t)m_faces.size() << endl;

	boost::unordered_map<HalfEdgeVertex<VertexT, NormalT>*, int> index_map;

	this->m_nVertices 		= (uint32_t)m_vertices.size();
	this->m_nFaces 			= (uint32_t)m_faces.size();

	this->m_vertexBuffer 	= new float[3 * this->m_nVertices];
	this->m_normalBuffer 	= new float[3 * this->m_nVertices];
	this->m_colorBuffer 	= new float[3 * this->m_nVertices];

	this->m_indexBuffer 	= new unsigned int[3 * this->m_nFaces];

	typename vector<HVertex*>::iterator vertices_iter = m_vertices.begin();
	typename vector<HVertex*>::iterator vertices_end = m_vertices.end();
	for(size_t i = 0; vertices_iter != vertices_end; ++i, ++vertices_iter)
	{
		this->m_vertexBuffer[3 * i] =     (*vertices_iter)->m_position[0];
		this->m_vertexBuffer[3 * i + 1] = (*vertices_iter)->m_position[1];
		this->m_vertexBuffer[3 * i + 2] = (*vertices_iter)->m_position[2];

		this->m_normalBuffer [3 * i] =     -(*vertices_iter)->m_normal[0];
		this->m_normalBuffer [3 * i + 1] = -(*vertices_iter)->m_normal[1];
		this->m_normalBuffer [3 * i + 2] = -(*vertices_iter)->m_normal[2];

		this->m_colorBuffer  [3 * i] = 0.8;
		this->m_colorBuffer  [3 * i + 1] = 0.8;
		this->m_colorBuffer  [3 * i + 2] = 0.8;

		// map the old index to the new index in the vertexBuffer
		index_map[*vertices_iter] = i;
	}

	typename vector<HalfEdgeFace<VertexT, NormalT>*>::iterator face_iter = m_faces.begin();
	typename vector<HalfEdgeFace<VertexT, NormalT>*>::iterator face_end  = m_faces.end();
	
	for(size_t i = 0; face_iter != face_end; ++i, ++face_iter)
	{
		this->m_indexBuffer[3 * i]      = index_map[(*(*face_iter))(0)];
		this->m_indexBuffer[3 * i + 1]  = index_map[(*(*face_iter))(1)];
		this->m_indexBuffer[3 * i + 2]  = index_map[(*(*face_iter))(2)];
		
		int surface_class = 1;
		surface_class = (*face_iter)->m_region;

		float r, g, b;
		if(m_colorRegions)
		{
		   r = fabs(cos(surface_class));
		   g = fabs(sin(surface_class * 30));
		   b = fabs(sin(surface_class * 2));
		}
		else
		{
		    r = 0.0;
		    g = 0.8;
		    b = 0.0;
		}
		this->m_colorBuffer[this->m_indexBuffer[3 * i]  * 3 + 0] = r;
		this->m_colorBuffer[this->m_indexBuffer[3 * i]  * 3 + 1] = g;
		this->m_colorBuffer[this->m_indexBuffer[3 * i]  * 3 + 2] = b;

		this->m_colorBuffer[this->m_indexBuffer[3 * i + 1] * 3 + 0] = r;
		this->m_colorBuffer[this->m_indexBuffer[3 * i + 1] * 3 + 1] = g;
		this->m_colorBuffer[this->m_indexBuffer[3 * i + 1] * 3 + 2] = b;

		this->m_colorBuffer[this->m_indexBuffer[3 * i + 2] * 3 + 0] = r;
		this->m_colorBuffer[this->m_indexBuffer[3 * i + 2] * 3 + 1] = g;
		this->m_colorBuffer[this->m_indexBuffer[3 * i + 2] * 3 + 2] = b;


	}

	this->m_finalized = true;
}

} // namespace lssr