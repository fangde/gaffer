//////////////////////////////////////////////////////////////////////////
//  
//  Copyright (c) 2014, Image Engine Design Inc. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//  
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//  
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  
//////////////////////////////////////////////////////////////////////////

// we undefine NDEBUG so we can use assert() for our test cases.
/// \todo We might like to define our own assert which throws an
/// exception which is designed to be caught by the python test
/// runner and reported nicely.
#undef NDEBUG

#include "tbb/tbb.h"

#include "Gaffer/Node.h"
#include "Gaffer/Metadata.h"

#include "GafferTest/MetadataTest.h"

using namespace tbb;
using namespace IECore;
using namespace Gaffer;

struct TestThreading
{

	void operator()( const blocked_range<size_t> &r ) const
	{
		for( size_t i=r.begin(); i!=r.end(); ++i )
		{
			NodePtr n = new Node();
			PlugPtr p = new Plug();
			
			assert( Metadata::nodeValue<Data>( n.get(), "threadingTest" ) == NULL );
			assert( Metadata::plugValue<Data>( p.get(), "threadingTest" ) == NULL );
			
			Metadata::registerNodeValue( n, "threadingTest", new IECore::IntData( 1 ) );
			Metadata::registerPlugValue( p, "threadingTest", new IECore::IntData( 2 ) );
			
			assert( Metadata::nodeValue<IntData>( n.get(), "threadingTest" )->readable() == 1 );
			assert( Metadata::plugValue<IntData>( p.get(), "threadingTest" )->readable() == 2 );
		}
	}

};

void GafferTest::testMetadataThreading()
{
	// this test simulates many different scripts being loaded
	// concurrently in separate threads, with each script registering
	// per-instance metadata for its members.
	TestThreading t;
	parallel_for( blocked_range<size_t>( 0, 10000 ), t );
}
