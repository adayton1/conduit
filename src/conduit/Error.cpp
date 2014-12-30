//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2014, Lawrence Livermore National Security, LLC
// Produced at the Lawrence Livermore National Laboratory. 
// 
// All rights reserved.
// 
// This source code cannot be distributed without further review from 
// Lawrence Livermore National Laboratory.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
///
/// file: Error.cpp
///
//-----------------------------------------------------------------------------

#include "Error.h"

//-----------------------------------------------------------------------------
// -- begin conduit:: --
//-----------------------------------------------------------------------------
namespace conduit
{

//-----------------------------------------------------------------------------
//
// -- conduit::Error public members --
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Error Construction and Destruction
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
Error::Error()
:m_msg(""),
 m_file(""),
 m_line(0)
{}

//---------------------------------------------------------------------------//
Error::Error(const Error &err)
:m_msg(err.m_msg),
 m_file(err.m_file),
 m_line(err.m_line)

{}

//---------------------------------------------------------------------------//
Error::Error(const std::string &msg,
             const std::string &file,
             index_t line)
:m_msg(msg),
 m_file(file),
 m_line(line)
{}

//---------------------------------------------------------------------------//
Error::~Error() throw()
{}

//-----------------------------------------------------------------------------
// Methods used to access and display Error as a human readable string. 
//-----------------------------------------------------------------------------


//---------------------------------------------------------------------------//
void
Error::print() const
{
    std::cout << message() << std::endl;
}


//---------------------------------------------------------------------------//
std::string
Error::message() const
{
    std::ostringstream oss;
    message(oss);
    return oss.str();
}

//---------------------------------------------------------------------------//
void
    Error::message(std::ostringstream &oss) const
{
    std::string msg = m_msg;
    if(msg == "")
        msg = "<EMPTY>";
    oss << "[" << m_file << ":" << m_line <<"]Error: " << msg;    
}
    


}
//-----------------------------------------------------------------------------
// -- end conduit:: --
//-----------------------------------------------------------------------------

