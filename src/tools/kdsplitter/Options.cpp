/**
 * Copyright (C) 2013 Universität Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */



#include "Options.hpp"

namespace lvr_kdsplitter
{

Options::Options(int argc, char** argv) : m_descr("Supported options")
{

	// Create option descriptions

	m_descr.add_options()
	("help", "Produce help message")
    ("inputFile", value< vector<string> >(), "Input file name. ")
    ("outputFile,o",    value<string>(&m_outputFile)->default_value("normals.ply"), "Output file name.")
    ("overlap,l",    value<float>(&m_overlap)->default_value(0.3), "Overlap size. Default: 0.3")
    ("leafSize,s",    value<size_t>(&m_leafsize)->default_value(100000), "Max Leaf Size. Default: 100000")
    ("first",    value<size_t>(&m_first)->default_value(-1), "First file number to parse")
    ("last",    value<size_t>(&m_last)->default_value(-1), "Last file number to parse")

    ;

    m_pdescr.add("inputFile", -1);

	// Parse command line and generate variables map
	store(command_line_parser(argc, argv).options(m_descr).positional(m_pdescr).run(), m_variables);
	notify(m_variables);

	if(m_variables.count("help"))
	{
		::std::cout << m_descr << ::std::endl;
        exit(-1);
	}

}



Options::~Options()
{
	// TODO Auto-generated destructor stub
}

}

