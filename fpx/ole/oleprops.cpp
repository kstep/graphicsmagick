//  ----------------------------------------------------------------------------
//  MODULE    : OLEPropertySet.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines an OLE Property Set object 
//  COMMENT   : Enhanced OLE objects are platform independent
//      SCCSID            : @(#)oleprops.cpp  1.4 10:45:23 12 Sep 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include  <stdio.h>
  #include  <stdlib.h>

#if defined(USE_LEGACY_INCLUDES)
#  include <iostream.h>
#  include <fstream.h>
#else
#  include <iostream>
#  include <fstream>
   using namespace std;
#endif

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEPropertySet_h
  #include  "oleprops.h"
#endif
#ifndef OLEStorages_h
  #include  "olestorg.h"
#endif
#ifndef OLEStreams_h
  #include  "olestrm.h"
#endif
#ifndef OLEProperties_h
  #include  "oleprop.h"
#endif
  
//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of OLEPropertySet
//  ----------------------------------------------------------------------------

OLEPropertySet::OLEPropertySet(const GUID& clsID, OLEStorage * parentStorage, LPSTREAM currentStream) : 
  OLEHeaderStream(clsID, parentStorage, currentStream)
{
  propSetHdr.cSections = 1;   // Only one section is allowed
  
  // Create a new section
  pPropSection = new OLEPropertySection(this, clsID);   
  
  streamStartPos = 0;       // CAUTION: no position shift in a standard property set
  Seek();             // Point to the beginning of the stream
}

OLEPropertySet::~OLEPropertySet()
{
  // Delete the property section 
  if ( pPropSection )
    delete pPropSection;
}

// Create a new property in a property set
Boolean OLEPropertySet::NewProperty( DWORD propID, DWORD propType, OLEProperty ** ppNewProperty )
{ 
  if ( pPropSection->NewProperty(propID, propType, ppNewProperty) )
    return true;
  
  return false;
}

// Get a property in a property set
Boolean OLEPropertySet::GetProperty(DWORD propID, OLEProperty ** ppNewProperty )
{
  if ( pPropSection->GetProperty(propID, ppNewProperty) )
    return true;
  
  return false;
}

// Delete a property in a property set
Boolean OLEPropertySet::DeleteProperty(DWORD propID )
{
  if ( pPropSection->DeleteProperty(propID) )
    return true;
  
  return false;
}

Boolean OLEPropertySet::GetSection(const GUID& sectionID, OLEPropertySection** propSet)
{
  if ( pPropSection->SectionID() == sectionID )  {
    *propSet = pPropSection; 
    return true;
  }

  return false;
}

// Save the entire property set
Boolean OLEPropertySet::Commit()
{
  // DAG
  Boolean status = TRUE;
  
  // Write stream header: must be done first, prior to the rest...
  status = WriteHeader();
  if(status == FALSE)
    return status;
  
  // Write the section
  pPropSection->Write();
  
  // Call the parent function
  OLEHeaderStream::Commit();
  
  return true;
}

// Read the entire property set
Boolean OLEPropertySet::Revert( )
{
  // Call the parent function
  OLEHeaderStream::Revert();
  
  // Read the section
  pPropSection->Read();
  
  return true;
}

// Display the entire property set
Boolean OLEPropertySet::Display( ofstream * pLogStream )
{
#if defined(__GNUC__)
  return false;
#else
  char  str[512]; 
  
  // Display the property set header
  
  sprintf(str, "Byte-order Indicator:   %04X", propSetHdr.wByteOrder); 
  pLogStream->write(str, strlen(str)); 
  pLogStream->put('\n'); 
  sprintf(str, "Format Version: %04X", propSetHdr.wFormat); 
  pLogStream->write(str, strlen(str)); 
  pLogStream->put('\n'); 
  sprintf(str, "OS Version: %04X", propSetHdr.dwOSVer); 
  pLogStream->write(str, strlen(str)); 
  pLogStream->put('\n'); 
  sprintf(str, "Section Numbers:    %i", propSetHdr.cSections);
  pLogStream->write(str, strlen(str)); 
  pLogStream->put('\n'); 
   
  // Display the properties in each section
  pPropSection->Display(pLogStream);
  
  return true;
#endif
}

//  ----------------------------------------------------------------------------
//  Methods of OLEPropertySection
//  ----------------------------------------------------------------------------

// Create a new property section with given classID
OLEPropertySection::OLEPropertySection( OLEPropertySet * parentPropSet, const GUID& theSecID )
{
  parPropSet    = parentPropSet;  // the property set to which this section belongs
  ppOLEProp     = NULL;       // property array: none for now
  sectionOffset = 0;        // section offset: we don't know for now
  sectionSize   = 0;        // section size: same thing
  numOfProp     = 0;        // initial number of properties
  secID       = theSecID;     // the Format ID

  // Create the code page property
  OLEProperty *pProp;
  NewProperty(PROPERTYIDONE, VT_I2, &pProp);  // Create a property
  *pProp = (short)CODEPAGE;         // Code page is 1200 right now
}

// Create a new property section with default classID
OLEPropertySection::OLEPropertySection( OLEPropertySet * parentPropSet )
{
  parPropSet    = parentPropSet;  // the property set to which this section belongs
  ppOLEProp     = NULL;       // property array: none for now
  sectionOffset = 0;        // section offset: we don't know for now
  sectionSize   = 0;        // section size: same thing
  numOfProp     = 0;        // initial number of properties 
  secID       = BlankGUID;    // the Format ID will be read latter...   
  
  // Create the code page property
  OLEProperty *pProp;
  NewProperty(PROPERTYIDONE, VT_I2, &pProp);  // Create a property 
  *pProp = (short)CODEPAGE;         // Code page is 1200 right now
}

OLEPropertySection::~OLEPropertySection( )
{
  if (ppOLEProp) {
  
    for ( long i= 0; i < (long) numOfProp; i++ )
      delete ppOLEProp[i]; 
      
    // Delete the property list array
    delete ppOLEProp;
  }
}

// Create a new property in a property section
Boolean OLEPropertySection::NewProperty( DWORD propID, DWORD propType, OLEProperty ** ppNewProperty )
{
  // If the property already exist, delete it first
  if ( GetProperty(propID, ppNewProperty, numOfProp) )
    DeleteProperty(propID);
    
  // If not, create a new OLE property
  if (!(*ppNewProperty = new OLEProperty(parPropSet, this, propID, propType)))
    return false;
    
  // Add this new property to the property array
  if ( !(Renew(*ppNewProperty, (short)++numOfProp)))
    return false; 

  return true;
}

// Get a property in a property section
Boolean OLEPropertySection::GetProperty(DWORD propID, OLEProperty ** ppNewProperty )
{
  for ( unsigned long i = 0; i< numOfProp; i++ )
    // If the property already exist, return it
    if ( ppOLEProp[i]->GetPropID() == propID ) {
      *ppNewProperty= ppOLEProp[i]; 
      return true;
    }
  
  return false;
}

// Get a property in a property section
Boolean OLEPropertySection::GetProperty(DWORD propID, OLEProperty ** ppNewProperty, DWORD propCount)
{
  for ( unsigned long i = 0; i< propCount; i++ )
    // If the property already exist, return it
    if ( ppOLEProp[i]->GetPropID() == propID ) { 
      *ppNewProperty= ppOLEProp[i]; 
      return true;
    }
  
  return false;
}

// Delete a property in a property section
Boolean OLEPropertySection::DeleteProperty(DWORD propID )
{
  for ( unsigned long i = 0; i< numOfProp; i++ )
    if ( ppOLEProp[i]->GetPropID() == propID ) 
    {   
      // Delete this property with propID
      delete ppOLEProp[i]; 
      
      // Reset the property array and minus total number of properties
      for ( DWORD j = i; j< numOfProp-1; j++ )
        ppOLEProp[j]= ppOLEProp[j+1];
      
      numOfProp--;  
      return true;
    }
  
  return false;
}

// Reallocate the property list array and store the new one in it
Boolean OLEPropertySection::Renew( OLEProperty* newProp, short propCount)
{
  // If property list array already exist, delete it first
  if ( ppOLEProp )
  {
    // Allocate a temporary property list array with new size
    OLEProperty** temp = new OLEProperty* [propCount]; 
    if (!temp)    return false; 
    
    // Transfer the data in old array into new one
    for ( long i = 0; i< propCount-1; i++ )   
      temp[i] = ppOLEProp[i]; 
      
    // Save the new property in last entry of array
    temp[propCount-1] = newProp;
    
    // Delete the old array and assign the temporary one
    delete[] ppOLEProp;  
    ppOLEProp = temp; 
  }
  else
  {
    // Allocate a new property list array with new size
    ppOLEProp = new OLEProperty* [propCount];   
    if (!ppOLEProp)   return false; 

    // Save the new property in last entry of array
    ppOLEProp[propCount-1] = newProp;
  } 

  return true;
}

// Set section size and offset
void OLEPropertySection::SetPropOffsetSecSize( )
{
  // set initial offset of properties type/value pairs
  unsigned long tempOff = sizeof(PROPERTYHEADER) * (numOfProp+1); 

  for ( DWORD i = 0; i< numOfProp; i++ )  {
    if (i)  
      // set the offset of properties type/value pairs
      tempOff = ppOLEProp[i-1]->GetPropOffset() + 
            ppOLEProp[i-1]->len + sizeof(DWORD);  
    
    ppOLEProp[i]->SetPropOffset(tempOff);
  }

  // set the section size
  sectionSize = ppOLEProp[numOfProp-1]->GetPropOffset() + sizeof(DWORD) +
          ppOLEProp[numOfProp-1]->len;      
}

// Read a property section
Boolean OLEPropertySection::Read()
{
  DWORD     properType, properOffset; 
  DWORD       *pProperID, *pProperOffset; 
  OLEProperty*  pNewProperty;
  DWORD       i;
    
  if ( ppOLEProp )  { delete [] ppOLEProp;  ppOLEProp = NULL; }

  // Read the section FMTID and offset
  parPropSet->ReadVT_CLSID(&secID); 
  parPropSet->ReadVT_I4(&sectionOffset);
  
  parPropSet->Seek(sectionOffset);
  
  // Read the section size and number of properties
  parPropSet->ReadVT_I4(&sectionSize);
  parPropSet->ReadVT_I4(&numOfProp);

  pProperID     = new DWORD [numOfProp];  
  if (pProperID==NULL) {
    return false;
  }
  pProperOffset   = new DWORD [numOfProp];  
  if (pProperOffset==NULL) {
    return false;
  }
  
  for ( i = 0; i < numOfProp; i++ ) {
    parPropSet->ReadVT_I4(&pProperID[i]);
    parPropSet->ReadVT_I4(&properOffset);
    pProperOffset[i] = properOffset;
  }
 
  for ( i = 0; i < numOfProp; i++ ) {
  
    // Set the stream pointer to the beginning of the property data
    parPropSet->Seek(sectionOffset + pProperOffset[i]);
    
    // FIX_DICT_PROP - If the property ID is 0, then this is a dictionary. A dictionary
    //  is different than other properties. it does not start with a type/value pair.
    //  So we must test for it and treat it as a special case.

    if(pProperID[i] == 0) {
      // Handle a dictionary property as a special case
      DWORD   numEntries; 
      
      // Read the number of dictionary entries
      parPropSet->ReadVT_I4(&numEntries);
      
      // Allocate a property object and it's array of entries
      if (!(pNewProperty = new OLEProperty(parPropSet, this, pProperID[i], DICT_PROP_TYPE)))
        return false;
      if (!(pNewProperty->pDict = AllocDICTIONARY(numEntries) ))
        return false;
      
      // Read the entries. Set 'len' to size of entries read
      pNewProperty->len = parPropSet->ReadDICT_ENTRIES(pNewProperty->pDict);
      if( !pNewProperty->len )
        return false;
    }
    else {
      
      // Read the property type indicator
      parPropSet->ReadVT_I4(&properType);
        
      if (!(pNewProperty = new OLEProperty(parPropSet, this, pProperID[i], properType)))
        return false;
      
      // Read the property type/values pair into variant structure
      pNewProperty->val.vt = (unsigned short) properType; 
      pNewProperty->len = parPropSet->ReadVT(&pNewProperty->val);
    }
    
    if ( !Renew(pNewProperty, (short)i+1))
      return false; 
  }
  // DAG PURE1 
  delete[] pProperID;
  delete[] pProperOffset;
  return true; 
}

// Write a property section
Boolean OLEPropertySection::Write()
{
  DWORD properID, properType, properOffset, i;
  DWORD startIDOff, startTyVal, propLen;
  
  // Write the section FMTID
  parPropSet->WriteVT_CLSID(&secID);
  
  // Calculate the section offset
  sectionOffset = sizeof(PROPERTYSETHEADER) + sizeof(SECTIONHEADER);  // Initial section offset

  parPropSet->WriteVT_I4(&sectionOffset);
  
  // Write the number of properties
  parPropSet->Seek(sizeof(DWORD), STREAM_SEEK_CUR );  // Skip the size of section which will be written later
  parPropSet->WriteVT_I4(&numOfProp); 
  
  // Initialize the offset of the property ID/offset pairs
  startIDOff = sizeof(PROPERTYHEADER) + sectionOffset; 
  
  // Initialize the offset of the property type/value pairs
  startTyVal = numOfProp * 2 * sizeof(DWORD) + startIDOff; 
  
  // Initialize the property offset 
  properOffset = startTyVal - sectionOffset; 

  // Write the properties ID and offset and property type/value pairs 
  for ( i = 0; i < numOfProp; i++ ) {
  
    // Set stream pointer to type/value pairs 
    parPropSet->Seek(startTyVal, STREAM_SEEK_SET );

    // FIX_DICT_PROP - Write either the dictionary or
    // Write the property type/value pairs
    properType = ppOLEProp[i]->GetPropType(); 
    if (properType == DICT_PROP_TYPE) {
    
      parPropSet->WriteVT_I4(&ppOLEProp[i]->pDict->cbEntries);  
      propLen = parPropSet->WriteDICT_ENTRIES(ppOLEProp[i]->pDict); 
    }
    else {
      parPropSet->WriteVT_I4(&properType);  
      propLen = parPropSet->WriteVT(&ppOLEProp[i]->val);  
    }
    
    // Set stream pointer to ID/Offset pairs 
    parPropSet->Seek(startIDOff, STREAM_SEEK_SET );

    //  Write the property ID/Offset pairs
    properID = ppOLEProp[i]->GetPropID(); 
    parPropSet->WriteVT_I4(&properID);  
    parPropSet->WriteVT_I4(&properOffset);  
    
    // Update the pointers 
    properOffset += sizeof(DWORD) + propLen; 
    startIDOff += 2 * sizeof(DWORD); 
    startTyVal +=   sizeof(DWORD) + propLen;
      
  }
  
  // Calculate the section size
  sectionSize = properOffset; 
  
  // Set stream pointer to section size 
  parPropSet->Seek(sectionOffset, STREAM_SEEK_SET );

  // Write the section size
  parPropSet->WriteVT_I4(&sectionSize); 

  // Set stream pointer to the new position 
  parPropSet->Seek(sectionOffset + sectionSize, STREAM_SEEK_SET );

  return true; 
}

// Display a property section
Boolean OLEPropertySection::Display( ofstream * pLogStream )
{
#if defined(__GNUC__)
  return false;
#else
  char  str[512]; 
  char *  strPtr = NULL; 
  DWORD properType; 
  
  for ( WORD i = 0; i< numOfProp; i++ ) {
    // Display property number and property ID
    sprintf(str, "%3i.  %08X  ", i+1, ppOLEProp[i]->GetPropID()); 
    pLogStream->write(str, strlen(str)); 
    
    // Display property type and property value
    properType = ppOLEProp[i]->GetPropType();
    sprintf(str, "%-20s ", VariantName(properType)); 
    pLogStream->write(str, strlen(str)); 
    
    parPropSet->VTtoString(&ppOLEProp[i]->val, &strPtr); 
    pLogStream->write(strPtr, strlen(strPtr)); 
    pLogStream->put('\n'); 
    delete strPtr;
  }
  
  return true;
#endif
}

