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
#include "halTopSegmentIterator.h"
#include "halBottomSegmentIterator.h"
#include "hdf5MetaData.h"


namespace hal {

class HDF5TopSegmentIterator;
class HDF5BottomSegmentIterator;
class HDF5DNAIterator;
class HDF5SequenceIterator;
class HDF5Alignment;
class HDF5Sequence;
/** 
 * HDF5 implementation of hal::Genome
 */
class HDF5Genome : public Genome
{
   friend class HDF5TopSegment;
   friend class HDF5TopSegmentIterator;
   friend class HDF5BottomSegment;
   friend class HDF5BottomSegmentIterator;
   friend class HDF5DNAIterator;
   friend class HDF5SequenceIterator;
public:

   HDF5Genome(const std::string& name,
              HDF5Alignment* alignment,
              H5::CommonFG* h5Parent,
              const H5::DSetCreatPropList& dcProps);

   virtual ~HDF5Genome();

   // GENOME INTERFACE

   const std::string& getName() const;

   void setDimensions(
     const std::vector<hal::Sequence::Info>& sequenceDimensions);

   void updateTopDimensions(
     const std::vector<hal::Sequence::UpdateInfo>& sequenceDimensions);

   void updateBottomDimensions(
     const std::vector<hal::Sequence::UpdateInfo>& sequenceDimensions);

   hal_size_t getNumSequences() const;
   
   Sequence* getSequence(const std::string& name);

   const Sequence* getSequence(const std::string& name) const;

   Sequence* getSequenceBySite(hal_size_t position);
   const Sequence* getSequenceBySite(hal_size_t position) const;
   
   SequenceIteratorPtr getSequenceIterator(
     hal_index_t position);

   SequenceIteratorConstPtr getSequenceIterator(
     hal_index_t position) const;

   SequenceIteratorConstPtr getSequenceEndIterator() const;

   MetaData* getMetaData();

   const MetaData* getMetaData() const;

   Genome* getParent();

   const Genome* getParent() const;

   Genome* getChild(hal_size_t childIdx);

   const Genome* getChild(hal_size_t childIdx) const;

   hal_size_t getNumChildren() const;

   hal_index_t getChildIndex(const Genome* child) const;

   // SEGMENTED SEQUENCE INTERFACE

   hal_size_t getSequenceLength() const;
   
   hal_size_t getNumTopSegments() const;

   hal_size_t getNumBottomSegments() const;

   TopSegmentIteratorPtr getTopSegmentIterator(
     hal_index_t position);

   TopSegmentIteratorConstPtr getTopSegmentIterator(
     hal_index_t position) const;

   TopSegmentIteratorConstPtr getTopSegmentEndIterator() const;

   BottomSegmentIteratorPtr getBottomSegmentIterator(
     hal_index_t position);

   BottomSegmentIteratorConstPtr getBottomSegmentIterator(
     hal_index_t position) const;

   BottomSegmentIteratorConstPtr getBottomSegmentEndIterator() const;

   DNAIteratorPtr getDNAIterator(hal_index_t position);

   DNAIteratorConstPtr getDNAIterator(hal_index_t position) const;

   DNAIteratorConstPtr getDNAEndIterator() const;

   void getString(std::string& outString) const;

   void setString(const std::string& inString);

   void getSubString(std::string& outString, hal_size_t start,
                             hal_size_t length) const;

   void setSubString(const std::string& intString, 
                             hal_size_t start,
                             hal_size_t length);

   RearrangementPtr getRearrangement(hal_index_t position) const;

   GappedTopSegmentIteratorConstPtr getGappedTopSegmentIterator(
     hal_index_t i, hal_size_t gapThreshold) const;

   GappedBottomSegmentIteratorConstPtr getGappedBottomSegmentIterator(
     hal_index_t i, hal_size_t childIdx, hal_size_t gapThreshold) const;


   // HDF5 SPECIFIC 
   void write();
   void read();
   void create();
   void resetTreeCache();

protected:

   void readSequences();
   void writeSequences(const std::vector<hal::Sequence::Info>&
                       sequenceDimensions);
   void deleteSequenceCache();
   void setGenomeTopDimensions(
     const std::vector<hal::Sequence::UpdateInfo>& sequenceDimensions);

   void setGenomeBottomDimensions(
     const std::vector<hal::Sequence::UpdateInfo>& sequenceDimensions);


protected:

   HDF5Alignment* _alignment;
   H5::CommonFG* _h5Parent;
   AlignmentPtr _alignmentPtr;
   std::string _name;
   HDF5MetaData* _metaData;
   HDF5ExternalArray _dnaArray;
   HDF5ExternalArray _topArray;
   HDF5ExternalArray _bottomArray;
   HDF5ExternalArray _sequenceArray;
   H5::Group _group;
   H5::DSetCreatPropList _dcprops;
   hal_size_t _numChildrenInBottomArray;

   mutable Genome* _parentCache;
   mutable std::vector<Genome*> _childCache;
   mutable std::map<hal_size_t, HDF5Sequence*> _sequencePosCache;
   mutable std::map<std::string, HDF5Sequence*> _sequenceNameCache;

   static const std::string dnaArrayName;
   static const std::string topArrayName;
   static const std::string bottomArrayName;
   static const std::string sequenceArrayName;
   static const std::string metaGroupName;
};


}
#endif

