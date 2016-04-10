//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2014-2015, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-666778
// 
// All rights reserved.
// 
// This file is part of Conduit. 
// 
// For details, see: http://llnl.github.io/conduit/.
// 
// Please also read conduit/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
///
/// file: Schema.cpp
///
//-----------------------------------------------------------------------------
#include "Schema.hpp"

//-----------------------------------------------------------------------------
// -- standard lib includes -- 
//-----------------------------------------------------------------------------
#include <stdio.h>

//-----------------------------------------------------------------------------
// -- conduit includes -- 
//-----------------------------------------------------------------------------
#include "Generator.hpp"
#include "Error.hpp"
#include "Utils.hpp"


//-----------------------------------------------------------------------------
// -- begin conduit:: --
//-----------------------------------------------------------------------------
namespace conduit
{

//=============================================================================
//-----------------------------------------------------------------------------
//
//
// -- begin conduit::Schema public methods --
//
//
//-----------------------------------------------------------------------------
//=============================================================================

//----------------------------------------------------------------------------
//
/// Schema construction and destruction.
//
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
Schema::Schema()
{
    init_defaults();
}

//---------------------------------------------------------------------------//
Schema::Schema(const Schema &schema)
{
    init_defaults();
    set(schema);
}

//---------------------------------------------------------------------------//
Schema::Schema(index_t dtype_id)
{
    init_defaults();
    set(dtype_id);
}

//---------------------------------------------------------------------------//
Schema::Schema(const DataType &dtype)
{
    init_defaults();
    set(dtype);
}


//---------------------------------------------------------------------------//
Schema::Schema(const std::string &json_schema)
{
    init_defaults();
    set(json_schema);
}

//---------------------------------------------------------------------------//
Schema::Schema(const char *json_schema)
{
    init_defaults();
    set(std::string(json_schema));
}


//---------------------------------------------------------------------------//
Schema::~Schema()
{release();}

//---------------------------------------------------------------------------//
void
Schema::reset()
{
    release();
}

//-----------------------------------------------------------------------------
//
// Schema set methods
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
void 
Schema::set(const Schema &schema)
{
    bool init_children = false;
    index_t dt_id = schema.m_dtype.id();
    if (dt_id == DataType::OBJECT_ID)
    {
       init_object();
       init_children = true;

       object_map() = schema.object_map();
       object_order() = schema.object_order();
    } 
    else if (dt_id == DataType::LIST_ID)
    {
       init_list();
       init_children = true;
    }
    else 
    {
        m_dtype = schema.m_dtype;
    }

    
    if (init_children) 
    {
       std::vector<Schema*> &my_children = children();
       const std::vector<Schema*> &their_children = schema.children();
       for (index_t i = 0; i < (index_t)their_children.size(); i++) 
       {
           Schema *child_schema = new Schema(*their_children[i]);
           child_schema->m_parent = this;
           my_children.push_back(child_schema);
       }
    }
}


//---------------------------------------------------------------------------//
void 
Schema::set(index_t dtype_id)
{
    reset();
    m_dtype.reset();
    m_dtype.set_id(dtype_id);
}


//---------------------------------------------------------------------------//
void 
Schema::set(const DataType &dtype)
{
    reset();
    if (dtype.id() == DataType::OBJECT_ID) {
        init_object();
    } else if (dtype.id() == DataType::LIST_ID) {
        init_list();
    }
    m_dtype = dtype;
}

//---------------------------------------------------------------------------//
void 
Schema::set(const std::string &json_schema)
{
    reset();
    walk_schema(json_schema);
}


//-----------------------------------------------------------------------------
//
/// Schema assignment operators
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
Schema &
Schema::operator=(const Schema &schema)
{
    if(this != &schema)
    {
        set(schema);
    }
    return *this;
}

//---------------------------------------------------------------------------//
Schema &
Schema::operator=(const DataType &dtype)
{
    set(dtype);
    return *this;
}


//---------------------------------------------------------------------------//
Schema &
Schema::operator=(const std::string &json_schema)
{
    set(json_schema);
    return *this;
}

//-----------------------------------------------------------------------------
//
/// Information Methods
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
index_t
Schema::total_bytes() const
{
    index_t res = 0;
    index_t dt_id = m_dtype.id();
    if(dt_id == DataType::OBJECT_ID || dt_id == DataType::LIST_ID)
    {
        const std::vector<Schema*> &lst = children();
        for (std::vector<Schema*>::const_iterator itr = lst.begin();
             itr < lst.end(); ++itr)
        {
            res += (*itr)->total_bytes();
        }
    }
    else if (dt_id != DataType::EMPTY_ID)
    {
        res = m_dtype.total_bytes();
    }
    return res;
}

//---------------------------------------------------------------------------//
index_t
Schema::total_bytes_compact() const
{
    index_t res = 0;
    index_t dt_id = m_dtype.id();
    if(dt_id == DataType::OBJECT_ID || dt_id == DataType::LIST_ID)
    {
        const std::vector<Schema*> &lst = children();
        for (std::vector<Schema*>::const_iterator itr = lst.begin();
             itr < lst.end(); ++itr)
        {
            res += (*itr)->total_bytes_compact();
        }
    }
    else if (dt_id != DataType::EMPTY_ID)
    {
        res = m_dtype.total_bytes_compact();
    }
    return res;
}

//---------------------------------------------------------------------------//
bool
Schema::is_compact() const
{
    return total_bytes_compact() == total_bytes();
}

//---------------------------------------------------------------------------//
index_t
Schema::spanned_bytes() const
{
    index_t res = 0;

    index_t dt_id = m_dtype.id();
    if(dt_id == DataType::OBJECT_ID || dt_id == DataType::LIST_ID)
    {
        const std::vector<Schema*> &lst = children();
        for (std::vector<Schema*>::const_iterator itr = lst.begin();
             itr < lst.end(); ++itr)
        {
            // spanned bytes is the max of the spanned bytes of 
            // all children
            index_t curr_span = (*itr)->spanned_bytes();
            if(curr_span > res)
            {
                res = curr_span;
            }
        }
    }
    else
    {
        res = m_dtype.spanned_bytes();
    }
    return res;
}


//---------------------------------------------------------------------------//
bool
Schema::compatible(const Schema &s) const
{
    index_t dt_id   = m_dtype.id();
    index_t s_dt_id = s.dtype().id();

    if(dt_id != s_dt_id)
        return false;
    
    bool res = true;
    
    if(dt_id == DataType::OBJECT_ID)
    {
        // each of s's entries that match paths must have dtypes that match
        
        std::map<std::string, index_t>::const_iterator itr;
        
        for(itr  = s.object_map().begin(); 
            itr != s.object_map().end() && res;
            itr++)
        {
            if(has_path(itr->first))
            {
                index_t s_idx = itr->second;
                res = s.children()[s_idx]->compatible(fetch_child(itr->first));
            }
        }
    }
    else if(dt_id == DataType::LIST_ID) 
    {
        // each of s's entries dtypes must match
        index_t s_n_chd = s.number_of_children();
        
        // can't be compatible in this case
        if(number_of_children() < s_n_chd)
            return false;

        const std::vector<Schema*> &s_lst = s.children();
        const std::vector<Schema*> &lst   = children();

        for(index_t i = 0; i < s_n_chd && res; i++)
        {
            res = lst[i]->compatible(*s_lst[i]);
        }
    }
    else
    {
        res = m_dtype.compatible(s.dtype());
    }
    return res;
}

//---------------------------------------------------------------------------//
bool
Schema::equal(const Schema &s) const
{
    index_t dt_id   = m_dtype.id();
    index_t s_dt_id = s.dtype().id();

    if(dt_id != s_dt_id)
        return false;
    
    bool res = true;
    
    if(dt_id == DataType::OBJECT_ID)
    {
        // all entries must be equal
        
        std::map<std::string, index_t>::const_iterator itr;
        
        for(itr  = s.object_map().begin(); 
            itr != s.object_map().end() && res;
            itr++)
        {
            if(has_path(itr->first))
            {
                index_t s_idx = itr->second;
                res = s.children()[s_idx]->equal(fetch_child(itr->first));
            }
            else
            {
                res = false;
            }
        }
        
        for(itr  = object_map().begin(); 
            itr != object_map().end() && res;
            itr++)
        {
            if(s.has_path(itr->first))
            {
                index_t idx = itr->second;
                res = children()[idx]->equal(s.fetch_child(itr->first));
            }
            else
            {
                res = false;
            }
        }
        
    }
    else if(dt_id == DataType::LIST_ID) 
    {
        // all entries must be equal
        index_t s_n_chd = s.number_of_children();
        
        // can't be compatible in this case
        if(number_of_children() != s_n_chd)
            return false;

        const std::vector<Schema*> &s_lst = s.children();
        const std::vector<Schema*> &lst   = children();

        for(index_t i = 0; i < s_n_chd && res; i++)
        {
            res = lst[i]->equal(*s_lst[i]);
        }
    }
    else
    {
        res = m_dtype.equal(s.dtype());
    }
    return res;
}



//-----------------------------------------------------------------------------
//
/// Transformation Methods
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
void    
Schema::compact_to(Schema &s_dest) const
{
    s_dest.reset();
    compact_to(s_dest,0);
}

//---------------------------------------------------------------------------//
std::string
Schema::to_json(bool detailed,
               index_t indent, 
               index_t depth,
               const std::string &pad,
               const std::string &eoe) const
{
   std::ostringstream oss;
   to_json_stream(oss,detailed,indent,depth,pad,eoe);
   return oss.str();
}

//---------------------------------------------------------------------------//
void
Schema::to_json_stream(std::ostream &os,
                       bool detailed, 
                       index_t indent, 
                       index_t depth,
                       const std::string &pad,
                       const std::string &eoe) const
{
    if(m_dtype.id() == DataType::OBJECT_ID)
    {
        os << eoe;
        utils::indent(os,indent,depth,pad);
        os << "{" << eoe;
    
        index_t nchildren = (index_t) children().size();
        for(index_t i=0; i < nchildren;i++)
        {
            utils::indent(os,indent,depth+1,pad);
            os << "\""<< object_order()[i] << "\": ";
            children()[i]->to_json_stream(os,detailed,indent,depth+1,pad,eoe);
            if(i < nchildren-1)
                os << ",";
            os << eoe;
        }
        utils::indent(os,indent,depth,pad);
        os << "}";
    }
    else if(m_dtype.id() == DataType::LIST_ID)
    {
        os << eoe;
        utils::indent(os,indent,depth,pad);
        os << "[" << eoe;
        
        index_t nchildren = (index_t) children().size();
        for(index_t i=0; i < nchildren;i++)
        {
            utils::indent(os,indent,depth+1,pad);
            children()[i]->to_json_stream(os,detailed,indent,depth+1,pad,eoe);
            if(i < nchildren-1)
                os << ",";
            os << eoe;
        }
        utils::indent(os,indent,depth,pad);
        os << "]";      
    }
    else // assume leaf data type
    {
        m_dtype.to_json_stream(os);
    }
}

//-----------------------------------------------------------------------------
//
/// Basic I/O methods
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
void            
Schema::save(const std::string &ofname,
             bool detailed, 
             index_t indent, 
             index_t depth,
             const std::string &pad,
             const std::string &eoe) const
{
    // TODO: this is ineff, get base class rep correct?
    std::ostringstream oss;
    to_json_stream(oss,detailed,indent,depth,pad,eoe);    

    std::ofstream ofile;
    ofile.open(ofname.c_str());
    if(!ofile.is_open())
        CONDUIT_ERROR("<Schema::save> failed to open: " << ofname);
    ofile << oss.str();
    ofile.close();
}

//---------------------------------------------------------------------------//
void
Schema::load(const std::string &ifname)
{
    std::ifstream ifile;
    ifile.open(ifname.c_str());
    if(!ifile.is_open())
        CONDUIT_ERROR("<Schema::load> failed to open: " << ifname);
    std::string res((std::istreambuf_iterator<char>(ifile)),
                     std::istreambuf_iterator<char>());
    set(res);
}



//-----------------------------------------------------------------------------
//
/// Access to children (object and list interface)
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
index_t 
Schema::number_of_children() const 
{
    if(m_dtype.id() != DataType::LIST_ID  &&
       m_dtype.id() != DataType::OBJECT_ID)
        return 0;
    return (index_t)children().size();
}



//---------------------------------------------------------------------------//
Schema &
Schema::child(index_t idx)
{
    return *children()[idx];
}

//---------------------------------------------------------------------------//
const Schema &
Schema::child(index_t idx) const
{
    return *children()[idx];
}


//---------------------------------------------------------------------------//
Schema *
Schema::child_ptr(index_t idx)
{
    return &child(idx);
}


//---------------------------------------------------------------------------//
void    
Schema::remove(index_t idx)
{
    index_t dtype_id = m_dtype.id();
    if(! (dtype_id == DataType::LIST_ID || dtype_id == DataType::OBJECT_ID))
    {
        CONDUIT_ERROR("<Schema::remove> Schema is not LIST_ID or OBJECT_ID, dtype is" 
                      << m_dtype.name());
    }
    
    std::vector<Schema*>  &chldrn = children();
    if(idx > (index_t) chldrn.size())
    {
        CONDUIT_ERROR("<Schema::remove> Invalid index:" 
                    << idx << ">" << chldrn.size() <<  "(list_size)");
    }

    if(dtype_id == DataType::OBJECT_ID)
    {
        // any index above the current needs to shift down by one
        for (index_t i = idx; i < (index_t) object_order().size(); i++)
        {
            object_map()[object_order()[i]]--;
        }
        
        object_map().erase(object_order()[idx]);
        object_order().erase(object_order().begin() + idx);
    }

    Schema* child = chldrn[idx];
    delete child;
    chldrn.erase(chldrn.begin() + idx);
}

//---------------------------------------------------------------------------//
Schema &
Schema::operator[](index_t idx)
{
    return child(idx);
}

//---------------------------------------------------------------------------//
const Schema &
Schema::operator[](index_t idx) const
{
    return child(idx);
}

//-----------------------------------------------------------------------------
//
/// Object interface methods
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
Schema&
Schema::fetch_child(const std::string &path)
{
    // fetch w/ path forces OBJECT_ID
    if(m_dtype.id() != DataType::OBJECT_ID)
        CONDUIT_ERROR("<Schema::child[OBJECT_ID]>: Schema is not OBJECT_ID");

    std::string p_curr;
    std::string p_next;
    utils::split_path(path,p_curr,p_next);

    index_t idx = child_index(p_curr);
    
    // check for parent
    if(p_curr == "..")
    {
        if(m_parent == NULL)// TODO: check for erro (no parent)
        {
            CONDUIT_ERROR("Tried to fetch non-existent parent Schema.")
        }
        else
        {
            return m_parent->fetch_child(p_next);
        }
    }
    
    if(p_next.empty())
    {
        return *children()[idx];
    }
    else
    {
        return children()[idx]->fetch_child(p_next);
    }
}


//---------------------------------------------------------------------------//
const Schema &
Schema::fetch_child(const std::string &path) const
{
    // fetch w/ path forces OBJECT_ID
    if(m_dtype.id() != DataType::OBJECT_ID)
        CONDUIT_ERROR("<Schema::child[OBJECT_ID]>: Schema is not OBJECT_ID");

    std::string p_curr;
    std::string p_next;
    utils::split_path(path,p_curr,p_next);

    // check for parent
    if(p_curr == "..")
    {
        if(m_parent != NULL) // TODO: check for erro (no parent)
           return m_parent->fetch_child(p_next);
    }

    index_t idx = child_index(p_curr);
    
    if(p_next.empty())
    {
        return *children()[idx];
    }
    else
    {
        return children()[idx]->fetch_child(p_next);
    }
}

//---------------------------------------------------------------------------//
index_t
Schema::child_index(const std::string &path) const
{
    // find p_curr with an iterator
    std::map<std::string, index_t>::const_iterator itr;
    itr = object_map().find(path);

    // error if child does not exist. 
    if(itr == object_map().end())
    {
        ///
        /// TODO: Full path errors would be nice here. 
        ///
        CONDUIT_ERROR("<Schema::child_index[OBJECT_ID]>"
                    << "Attempt to access invalid child:" << path);
                      
    }

    return itr->second;
}

//---------------------------------------------------------------------------//
Schema &
Schema::fetch(const std::string &path)
{
    // fetch w/ path forces OBJECT_ID
    init_object();
        
    std::string p_curr;
    std::string p_next;
    utils::split_path(path,p_curr,p_next);

    // handle parent 
    // check for parent
    if(p_curr == "..")
    {
        if(m_parent != NULL) // TODO: check for error (no parent)
           return m_parent->fetch(p_next);
    }
    
    if (!has_path(p_curr)) 
    {
        Schema* my_schema = new Schema();
        my_schema->m_parent = this;
        children().push_back(my_schema);
        object_map()[p_curr] = children().size() - 1;
        object_order().push_back(p_curr);
    }

    index_t idx = child_index(p_curr);
    if(p_next.empty())
    {
        return *children()[idx];
    }
    else
    {
        return children()[idx]->fetch(p_next);
    }

}



//---------------------------------------------------------------------------//
Schema *
Schema::fetch_ptr(const std::string &path)
{
    return &fetch(path);
}


//---------------------------------------------------------------------------//
const Schema &
Schema::operator[](const std::string &path) const
{
    return fetch_child(path);
}

//---------------------------------------------------------------------------//
Schema &
Schema::operator[](const std::string &path)
{
    return fetch(path);
}

//---------------------------------------------------------------------------//
bool           
Schema::has_path(const std::string &path) const
{
    if(m_dtype.id() == DataType::EMPTY_ID)
        return false;
    if(m_dtype.id() != DataType::OBJECT_ID)
        CONDUIT_ERROR("<Schema::has_path[OBJECT_ID]> Schema is not OBJECT_ID");

    std::string p_curr;
    std::string p_next;
    utils::split_path(path,p_curr,p_next);
    
    // handle parent case (..)
    
    const std::map<std::string,index_t> &ents = object_map();

    if(ents.find(p_curr) == ents.end())
    {
        return false;
    }

    if(!p_next.empty())
    {
        index_t idx = ents.find(p_curr)->second;
        return children()[idx]->has_path(p_next);
    }
    else
    {
        return true;
    }
}


//---------------------------------------------------------------------------//
void
Schema::paths(std::vector<std::string> &paths) const
{
    paths = object_order();
}



//---------------------------------------------------------------------------//
void    
Schema::remove(const std::string &path)
{
    if(m_dtype.id() != DataType::OBJECT_ID)
        CONDUIT_ERROR("<Schema::remove[OBJECT_ID]> Schema is not OBJECT_ID");

    std::string p_curr;
    std::string p_next;
    utils::split_path(path,p_curr,p_next);
    index_t idx = child_index(p_curr);
    Schema *child = children()[idx];

    if(!p_next.empty())
    {
        child->remove(p_next);
    }
    else
    {
        // any index above the current needs to shift down by one
        for (index_t i = idx; i < (index_t) object_order().size(); i++)
        {
            object_map()[object_order()[i]]--;
        }
        object_map().erase(p_curr);
        object_order().erase(object_order().begin() + idx);
        children().erase(children().begin() + idx);
        delete child;
    }    
}

//---------------------------------------------------------------------------//
Schema &
Schema::append()
{
    init_list();
    init_list();
    Schema *sch = new Schema();
    children().push_back(sch);
    return *sch;
}


//=============================================================================
//-----------------------------------------------------------------------------
//
//
// -- begin conduit::Schema private methods --
//
//
//-----------------------------------------------------------------------------
//=============================================================================


//-----------------------------------------------------------------------------
//
// -- private methods that help with init, memory allocation, and cleanup --
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
void
Schema::init_defaults()
{
    m_dtype  = DataType::empty();
    m_hierarchy_data = NULL;
    m_parent = NULL;
}

//---------------------------------------------------------------------------//
void
Schema::init_object()
{
    if(dtype().id() != DataType::OBJECT_ID)
    {
        reset();
        m_dtype  = DataType::object();
        m_hierarchy_data = new Schema_Object_Hierarchy();
    }
}

//---------------------------------------------------------------------------//
void
Schema::init_list()
{
    if(dtype().id() != DataType::LIST_ID)
    {
        reset();
        m_dtype  = DataType::list();
        m_hierarchy_data = new Schema_List_Hierarchy();
    }
}


//---------------------------------------------------------------------------//
void
Schema::release()
{
    if(dtype().id() == DataType::OBJECT_ID ||
       dtype().id() == DataType::LIST_ID)
    {
        std::vector<Schema*> chld = children();
        for(index_t i=0; i< (index_t)chld.size(); i++)
        {
            delete chld[i];
        }
    }
    
    if(dtype().id() == DataType::OBJECT_ID)
    { 
        delete object_hierarchy();
    }
    else if(dtype().id() == DataType::LIST_ID)
    { 
        delete list_hierarchy();
    }

    m_dtype  = DataType::empty();
    m_hierarchy_data = NULL;
}



//-----------------------------------------------------------------------------
//
/// -- Private transform helpers -- 
//
//-----------------------------------------------------------------------------



//---------------------------------------------------------------------------//
void    
Schema::compact_to(Schema &s_dest, index_t curr_offset) const
{
    index_t dtype_id = m_dtype.id();
    
    if(dtype_id == DataType::OBJECT_ID )
    {
        index_t nchildren = children().size();
        for(index_t i=0; i < nchildren;i++)
        {
            Schema  *cld_src = children()[i];
            Schema &cld_dest = s_dest.fetch(object_order()[i]);
            cld_src->compact_to(cld_dest,curr_offset);
            curr_offset += cld_dest.total_bytes();
        }
    }
    else if(dtype_id == DataType::LIST_ID)
    {
        index_t nchildren = children().size();
        for(index_t i=0; i < nchildren ;i++)
        {            
            Schema  *cld_src = children()[i];
            Schema &cld_dest = s_dest.append();
            cld_src->compact_to(cld_dest,curr_offset);
            curr_offset += cld_dest.total_bytes();
        }
    }
    else if (dtype_id != DataType::EMPTY_ID)
    {
        // create a compact data type
        m_dtype.compact_to(s_dest.m_dtype);
        s_dest.m_dtype.set_offset(curr_offset);
    }
}




//---------------------------------------------------------------------------//
void 
Schema::walk_schema(const std::string &json_schema)
{
    Generator g(json_schema);
    g.walk(*this);
}


//-----------------------------------------------------------------------------
//
/// -- Private methods that help with access book keeping data structures. --
//
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
Schema::Schema_Object_Hierarchy *
Schema::object_hierarchy()
{
    return static_cast<Schema_Object_Hierarchy*>(m_hierarchy_data);
}

//---------------------------------------------------------------------------//
Schema::Schema_List_Hierarchy *
Schema::list_hierarchy()
{
    return static_cast<Schema_List_Hierarchy*>(m_hierarchy_data);
}


//---------------------------------------------------------------------------//
const Schema::Schema_Object_Hierarchy *
Schema::object_hierarchy() const 
{
    return static_cast<Schema_Object_Hierarchy*>(m_hierarchy_data);
}

//---------------------------------------------------------------------------//
const Schema::Schema_List_Hierarchy *
Schema::list_hierarchy() const 
{
    return static_cast<Schema_List_Hierarchy*>(m_hierarchy_data);
}


//---------------------------------------------------------------------------//
std::vector<Schema*> &
Schema::children()
{
    if (m_dtype.id() == DataType::OBJECT_ID)
    {
        return object_hierarchy()->children;
    } 
    else 
    {
        return list_hierarchy()->children;
    }
}

//---------------------------------------------------------------------------//
std::map<std::string, index_t> &
Schema::object_map()
{
    return object_hierarchy()->object_map;
}


//---------------------------------------------------------------------------//
std::vector<std::string> &
Schema::object_order()
{
    return object_hierarchy()->object_order;
}

//---------------------------------------------------------------------------//
const std::vector<Schema*> &
Schema::children() const
{
    if (m_dtype.id() == DataType::OBJECT_ID)
    {
        return object_hierarchy()->children;
    } 
    else 
    {
        return list_hierarchy()->children;
    }
}

//---------------------------------------------------------------------------//
const std::map<std::string, index_t> &
Schema::object_map() const
{
    return object_hierarchy()->object_map;
}


//---------------------------------------------------------------------------//
const std::vector<std::string> &
Schema::object_order() const
{
    return object_hierarchy()->object_order;
}

//---------------------------------------------------------------------------//
void
Schema::object_map_print() const
{
    index_t sz = object_order().size();
    for(index_t i=0;i<sz;i++)
    {
        std::cout << object_order()[i] << " ";
    }
    std::cout << std::endl;
}


//---------------------------------------------------------------------------//
void
Schema::object_order_print() const
{
    std::map<std::string, index_t>::const_iterator itr; 
        
    for(itr = object_map().begin(); itr != object_map().end();itr++)
    {
       std::cout << itr->first << ":" << itr->second << " ";
    }
    std::cout << std::endl;
}


}
//-----------------------------------------------------------------------------
// -- end conduit:: --
//-----------------------------------------------------------------------------

