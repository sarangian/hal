/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HDF5GENOME_H
#define _HDF5GENOME_H

#include <H5Cpp.h>
#include "halGenome.h"
#include "hdf5ExternalArray.h"
#include "hdf5Alignment.h"
#include "halSegmentIterator.h"
#include "hdf5MetaData.h"

namespace hal {

/** 
 * HDF5 implementation of hal::Genome
 */
class HDF5Genome : public Genome
{
public:

   HDF5Genome(const std::string& name,
              HDF5Alignment* alignment,
              H5::CommonFG* h5Parent,
              const H5::DSetCreatPropList& dcProps);

   virtual ~HDF5Genome();

   void reset(hal_size_t totalSequenceLength,
              hal_size_t numTopSegments,
              hal_size_t numBottomSegments);

   void resetTopSegments(hal_size_t numTopSegments);

   void resetBottomSegments(hal_size_t numBottomSegments);

   const std::string& getName() const;
   AlignmentPtr getAlignment();
   hal_size_t getSequenceLength() const;
   hal_size_t getNumberTopSegments() const;
   hal_size_t getNumberBottomSegments() const;
   SegmentIteratorPtr getSegmentIterator(hal_bool_t top, 
                                         hal_index_t position);
   SegmentIteratorConstPtr getSegmentIterator(
     hal_bool_t top, hal_index_t position) const;
   
   MetaDataPtr getMetaData();
   MetaDataConstPtr getMetaData() const;

   void write();
   void read();
   void create();

protected:

   HDF5Alignment* _alignment;
   H5::CommonFG* _h5Parent;
   AlignmentPtr _alignmentPtr;
   std::string _name;
   MetaDataPtr _metaData;
   HDF5ExternalArray _dnaArray;
   HDF5ExternalArray _topArray;
   HDF5ExternalArray _bottomArray;
   GenomePtr _parent;
   std::vector<GenomePtr> _children;
   H5::Group _group;
   H5::DSetCreatPropList _dcprops;
   static const std::string dnaArrayName;
   static const std::string topArrayName;
   static const std::string bottomArrayName;
   static const std::string metaGroupName;
   
};

}
#endif
