//  ----------------------------------------------------------------------------
//  MODULE    : ImageInfoIO.cpp 
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT
//  MODIFIOR  : Yue Zhang 
//  DATE    : Thursday, February 22, 1996 
//  DESCRIPTION : This file contains functions implementing FlashPixIO functions 
//          in FPXBaselineIO.h
//  COMMENTS  : Functions in the file support read and write 9 property groups
//          in Image Info Property set 
//
//      SCCSID          : @(#)imginfio.cpp  1.6 13:18:07 02 Jun 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "imginfio.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEProperties_h
  #include  "oleprop.h"
#endif

#ifndef FileFlashPixIO_h
  #include "f_fpxio.h"
#endif
#ifndef FPXImageView_h
  #include "fpximgvw.h"
#endif
#ifndef FlashPixFormat_h
  #include "fpxformt.h"
#endif
#ifndef FlashPixUtils_h
  #include "fpxutils.h"
#endif

//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  External Functions
//  This is the interface of the Baseline FlashPix IO Toolkit
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data): File Source Group
//  ----------------------------------------------------------------------------

FPXStatus FPX_SetSourceGroup ( 
        FPXImageHandle*     theFPX,
          FPXFileSourceGroup* theSourceGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (theSourceGroup->file_source_valid)
      if (filePtr->SetImageInfoProperty (PID_FileSource, TYP_FileSource, &aProp))
        *aProp = (long)(theSourceGroup->file_source);

    if (theSourceGroup->scene_type_valid)
      if (filePtr->SetImageInfoProperty (PID_SceneType, TYP_SceneType, &aProp))
        *aProp = (long)(theSourceGroup->scene_type);

    if (theSourceGroup->creation_path_valid)
      if (filePtr->SetImageInfoProperty (PID_CreationPath, TYP_CreationPath, &aProp))
        *aProp = (FPXLongArray)theSourceGroup->creation_path;

    if (theSourceGroup->name_man_release_valid)
      if (filePtr->SetImageInfoProperty (PID_SoftwareName, TYP_SoftwareName, &aProp))
        *aProp = (FPXWideStr)theSourceGroup->name_man_release; 

    if (theSourceGroup->user_defined_id_valid)
      if (filePtr->SetImageInfoProperty (PID_UserDefinedID, TYP_UserDefinedID, &aProp))
        *aProp = (FPXWideStr)theSourceGroup->user_defined_id;

    if (theSourceGroup->original_sharpness_approximation_valid)
      if (filePtr->SetImageInfoProperty (PID_Sharpness, TYP_Sharpness, &aProp))
        *aProp = (float)(theSourceGroup->original_sharpness_approximation);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetSourceGroup ( 
        FPXImageHandle*     theFPX,
        FPXFileSourceGroup* theSourceGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetImageInfoProperty (PID_FileSource, &aProp)) {
      theSourceGroup->file_source = (FPXSource)((long)(*aProp));
      if (theSourceGroup->file_source > FPX_COMPUTER_GRAPHICS)
        theSourceGroup->file_source = FPX_UNIDENTIFIED_SOURCE;
      theSourceGroup->file_source_valid = true;
    } else
      theSourceGroup->file_source_valid = false;

    if (filePtr->GetImageInfoProperty (PID_SceneType, &aProp)) {
      theSourceGroup->scene_type = (FPXScene)((long)(*aProp));
      if (theSourceGroup->scene_type > FPX_DIGITAL_SCENE_GENERATION)
        theSourceGroup->scene_type = FPX_UNIDENTIFIED_SCENE; 
      theSourceGroup->scene_type_valid = true;
    } else
      theSourceGroup->scene_type_valid = false;

    if (filePtr->GetImageInfoProperty (PID_CreationPath, &aProp)) {
      theSourceGroup->creation_path = (FPXLongArray)(*aProp);
      theSourceGroup->creation_path_valid = true;
    } else
      theSourceGroup->creation_path_valid = false;

    if (filePtr->GetImageInfoProperty (PID_SoftwareName, &aProp)) {
      theSourceGroup->name_man_release = (FPXWideStr)(*aProp);
      theSourceGroup->name_man_release_valid = true;
    } else
      theSourceGroup->name_man_release_valid = false;

    if (filePtr->GetImageInfoProperty (PID_UserDefinedID, &aProp)) {
      theSourceGroup->user_defined_id = (FPXWideStr)(*aProp);
      theSourceGroup->user_defined_id_valid = true;
    } else
      theSourceGroup->user_defined_id_valid = false;

    if (filePtr->GetImageInfoProperty (PID_Sharpness, &aProp)) {
      theSourceGroup->original_sharpness_approximation = (float)(*aProp);
      theSourceGroup->original_sharpness_approximation_valid = true;
    } else
      theSourceGroup->original_sharpness_approximation_valid = false;


  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

//  ----------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data): Intellectual Property Group
//  ----------------------------------------------------------------------------

FPXStatus FPX_SetIntellectualPropGroup ( 
        FPXImageHandle*     theFPX,
              FPXIntellectualPropertyGroup*  thePropGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (thePropGroup->copyright_valid)
      if (filePtr->SetImageInfoProperty (PID_Copyright, TYP_Copyright, &aProp))
        *aProp = (FPXWideStr)(thePropGroup->copyright);

    if (thePropGroup->legal_broker_for_orig_image_valid)
      if (filePtr->SetImageInfoProperty (PID_OriginalBroker, TYP_OriginalBroker, &aProp))
        *aProp = (FPXWideStr)(thePropGroup->legal_broker_for_orig_image);

    if (thePropGroup->legal_broker_for_digital_image_valid)
      if (filePtr->SetImageInfoProperty (PID_DigitalBroker, TYP_DigitalBroker, &aProp))
        *aProp = (FPXWideStr)(thePropGroup->legal_broker_for_digital_image);

    if (thePropGroup->authorship_valid)
      if (filePtr->SetImageInfoProperty (PID_Authorship, TYP_Authorship, &aProp))
        *aProp = (FPXWideStr)(thePropGroup->authorship);

    if (thePropGroup->intellectual_prop_notes_valid)
      if (filePtr->SetImageInfoProperty (PID_PropertyNotes, TYP_PropertyNotes, &aProp))
        *aProp = (FPXWideStr)(thePropGroup->intellectual_prop_notes);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetIntellectualPropGroup ( 
        FPXImageHandle*     theFPX,
        FPXIntellectualPropertyGroup*     thePropGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetImageInfoProperty (PID_Copyright, &aProp)) {
      thePropGroup->copyright = (FPXWideStr)(*aProp);
      thePropGroup->copyright_valid = true;
    } else
      thePropGroup->copyright_valid = false;

    if (filePtr->GetImageInfoProperty (PID_OriginalBroker, &aProp)) {
      thePropGroup->legal_broker_for_orig_image = (FPXWideStr)(*aProp);
      thePropGroup->legal_broker_for_orig_image_valid = true;
    } else
      thePropGroup->legal_broker_for_orig_image_valid = false;

    if (filePtr->GetImageInfoProperty (PID_DigitalBroker, &aProp)) {
      thePropGroup->legal_broker_for_digital_image = (FPXWideStr)(*aProp);
      thePropGroup->legal_broker_for_digital_image_valid = true;
    } else
      thePropGroup->legal_broker_for_digital_image_valid = false;

    if (filePtr->GetImageInfoProperty (PID_Authorship, &aProp)) {
      thePropGroup->authorship = (FPXWideStr)(*aProp);
      thePropGroup->authorship_valid = true;
    } else
      thePropGroup->authorship_valid = false;

    if (filePtr->GetImageInfoProperty (PID_PropertyNotes, &aProp)) {
      thePropGroup->intellectual_prop_notes = (FPXWideStr)(*aProp);
      thePropGroup->intellectual_prop_notes_valid = true;
    } else
      thePropGroup->intellectual_prop_notes_valid = false;


  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

//  ----------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data): Content Description Group
//  ----------------------------------------------------------------------------

FPXStatus FPX_SetContentDescriptionGroup (
        FPXImageHandle*     theFPX,
              FPXContentDescriptionGroup*    theContentGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (theContentGroup->test_target_in_the_image_valid)
      if (filePtr->SetImageInfoProperty (PID_TestTargetType, TYP_TestTargetType, &aProp))
        *aProp = (unsigned long)(theContentGroup->test_target_in_the_image);

    if (theContentGroup->group_caption_valid)
      if (filePtr->SetImageInfoProperty (PID_GroupCaption, TYP_GroupCaption, &aProp))
        *aProp = (FPXWideStr)(theContentGroup->group_caption);

    if (theContentGroup->caption_text_valid)
      if (filePtr->SetImageInfoProperty (PID_CaptionText, TYP_CaptionText, &aProp))
        *aProp = (FPXWideStr)(theContentGroup->caption_text);

    if (theContentGroup->people_in_the_image_valid)
      if (filePtr->SetImageInfoProperty (PID_PeopleInTheImage, TYP_PeopleInTheImage, &aProp))
        *aProp = (FPXWideStrArray)(theContentGroup->people_in_the_image);

    if (theContentGroup->things_in_image_valid)
      if (filePtr->SetImageInfoProperty (PID_ThingsInTheImage, TYP_ThingsInTheImage, &aProp))
        *aProp = (FPXWideStrArray)(theContentGroup->things_in_image);

    if (theContentGroup->date_of_original_image_valid)
      if (filePtr->SetImageInfoProperty (PID_DateOfTheOriginal, TYP_DateOfTheOriginal, &aProp))
        *aProp = (FILETIME)(theContentGroup->date_of_original_image);

    if (theContentGroup->events_in_the_image_valid)
      if (filePtr->SetImageInfoProperty (PID_EventsInTheImage, TYP_EventsInTheImage, &aProp))
        *aProp = (FPXWideStrArray)(theContentGroup->events_in_the_image);

    if (theContentGroup->places_in_the_valid)
      if (filePtr->SetImageInfoProperty (PID_PlacesInTheImage, TYP_PlacesInTheImage, &aProp))
        *aProp = (FPXWideStrArray)(theContentGroup->places_in_the);

    if (theContentGroup->content_description_notes_valid)
      if (filePtr->SetImageInfoProperty (PID_DescriptionNotes, TYP_DescriptionNotes, &aProp))
        *aProp = (FPXWideStr)(theContentGroup->content_description_notes);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetContentDescriptionGroup (
        FPXImageHandle*     theFPX,
              FPXContentDescriptionGroup*    theContentGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetImageInfoProperty (PID_TestTargetType, &aProp)) {
      theContentGroup->test_target_in_the_image = (unsigned long)(*aProp);
      theContentGroup->test_target_in_the_image_valid = true;
    } else
      theContentGroup->test_target_in_the_image_valid = false;

    if (filePtr->GetImageInfoProperty (PID_GroupCaption, &aProp)) {
      theContentGroup->group_caption = (FPXWideStr)(*aProp);
      theContentGroup->group_caption_valid = true;
    } else
      theContentGroup->group_caption_valid = false;

    if (filePtr->GetImageInfoProperty (PID_CaptionText, &aProp)) {
      theContentGroup->caption_text = (FPXWideStr)(*aProp);
      theContentGroup->caption_text_valid = true;
    } else
      theContentGroup->caption_text_valid = false;

    if (filePtr->GetImageInfoProperty (PID_PeopleInTheImage, &aProp)) {
      theContentGroup->people_in_the_image = (FPXWideStrArray)(*aProp);
      theContentGroup->people_in_the_image_valid = true;
    } else
      theContentGroup->people_in_the_image_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ThingsInTheImage, &aProp)) {
      theContentGroup->things_in_image = (FPXWideStrArray)(*aProp);
      theContentGroup->things_in_image_valid = true;
    } else
      theContentGroup->things_in_image_valid = false;

    if (filePtr->GetImageInfoProperty (PID_DateOfTheOriginal, &aProp)) {
      theContentGroup->date_of_original_image = (FILETIME)(*aProp);
      theContentGroup->date_of_original_image_valid = true;
    } else
      theContentGroup->date_of_original_image_valid = false;

    if (filePtr->GetImageInfoProperty (PID_EventsInTheImage, &aProp)) {
      theContentGroup->events_in_the_image = (FPXWideStrArray)(*aProp);
      theContentGroup->events_in_the_image_valid = true;
    } else
      theContentGroup->events_in_the_image_valid = false;

    if (filePtr->GetImageInfoProperty (PID_PlacesInTheImage, &aProp)) {
      theContentGroup->places_in_the = (FPXWideStrArray)(*aProp);
      theContentGroup->places_in_the_valid = true;
    } else
      theContentGroup->places_in_the_valid = false;

    if (filePtr->GetImageInfoProperty (PID_DescriptionNotes, &aProp)) {
      theContentGroup->content_description_notes = (FPXWideStr)(*aProp);
      theContentGroup->content_description_notes_valid = true;
    } else
      theContentGroup->content_description_notes_valid = false;

  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}
              
//  ----------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data): Camera Information Group
//  ----------------------------------------------------------------------------

FPXStatus FPX_SetCameraInformationGroup (
        FPXImageHandle*               theFPX,
              FPXCameraInformationGroup*    theCameraGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (theCameraGroup->camera_manufacturer_name_valid)
      if (filePtr->SetImageInfoProperty (PID_CameraManufacturer, TYP_CameraManufacturer, &aProp))
        *aProp = (FPXWideStr)(theCameraGroup->camera_manufacturer_name);

    if (theCameraGroup->camera_model_name_valid)
      if (filePtr->SetImageInfoProperty (PID_CameraModel, TYP_CameraModel, &aProp))
        *aProp = (FPXWideStr)(theCameraGroup->camera_model_name);

    if (theCameraGroup->camera_serial_number_valid)
      if (filePtr->SetImageInfoProperty (PID_CameraSerialNumber, TYP_CameraSerialNumber, &aProp))
        *aProp = (FPXWideStr)(theCameraGroup->camera_serial_number);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetCameraInformationGroup (
        FPXImageHandle*               theFPX,
              FPXCameraInformationGroup*    theCameraGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetImageInfoProperty (PID_CameraManufacturer, &aProp)) {
      theCameraGroup->camera_manufacturer_name = (FPXWideStr)(*aProp);
      theCameraGroup->camera_manufacturer_name_valid = true;
    } else
      theCameraGroup->camera_manufacturer_name_valid = false;

    if (filePtr->GetImageInfoProperty (PID_CameraModel, &aProp)) {
      theCameraGroup->camera_model_name = (FPXWideStr)(*aProp);
      theCameraGroup->camera_model_name_valid = true;
    } else
      theCameraGroup->camera_model_name_valid = false;

    if (filePtr->GetImageInfoProperty (PID_CameraSerialNumber, &aProp)) {
      theCameraGroup->camera_serial_number = (FPXWideStr)(*aProp);
      theCameraGroup->camera_serial_number_valid = true;
    } else
      theCameraGroup->camera_serial_number_valid = false;

  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

//  ------------------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data): Per Picture Camera Settings Group
//  ------------------------------------------------------------------------------------

FPXStatus FPX_SetPerPictureGroup (
        FPXImageHandle*                      theFPX,
              FPXPerPictureCameraSettingsGroup*    thePerPictureGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    // capture_date is not consistent with TYP_CaptureDate
    if (thePerPictureGroup->capture_date_valid)
      if (filePtr->SetImageInfoProperty (PID_CaptureDate, TYP_CaptureDate, &aProp))
        *aProp = (FILETIME)(thePerPictureGroup->capture_date);

    if (thePerPictureGroup->exposure_time_valid)
      if (filePtr->SetImageInfoProperty (PID_ExposureTime, TYP_ExposureTime, &aProp))
        *aProp = (float)(thePerPictureGroup->exposure_time);

    if (thePerPictureGroup->f_number_valid)
      if (filePtr->SetImageInfoProperty (PID_F_Number, TYP_F_Number, &aProp))
        *aProp = (float)(thePerPictureGroup->f_number);

    if (thePerPictureGroup->exposure_program_valid)
      if (filePtr->SetImageInfoProperty (PID_ExposureProgram, TYP_ExposureProgram, &aProp))
        *aProp = (unsigned long)(FPXExposureProgram)(thePerPictureGroup->exposure_program);

    if (thePerPictureGroup->brightness_value_valid)
      if (filePtr->SetImageInfoProperty (PID_BrightnessValue, TYP_BrightnessValue, &aProp))
        *aProp = (FPXRealArray)(thePerPictureGroup->brightness_value);

    if (thePerPictureGroup->exposure_bias_value_valid)
      if (filePtr->SetImageInfoProperty (PID_ExposureBiasValue, TYP_ExposureBiasValue, &aProp))
        *aProp = (float)(thePerPictureGroup->exposure_bias_value);

    if (thePerPictureGroup->subject_distance_valid)
      if (filePtr->SetImageInfoProperty (PID_SubjectDistance, TYP_SubjectDistance, &aProp))
        *aProp = (FPXRealArray)(thePerPictureGroup->subject_distance);

    if (thePerPictureGroup->metering_mode_valid)
      if (filePtr->SetImageInfoProperty (PID_MeteringMode, TYP_MeteringMode, &aProp))
        *aProp = (unsigned long)(FPXMeteringMode)(thePerPictureGroup->metering_mode);

    if (thePerPictureGroup->scene_illuminant_valid)
      if (filePtr->SetImageInfoProperty (PID_SceneIlluminant, TYP_SceneIlluminant, &aProp))
        *aProp = (unsigned long)(FPXSceneIlluminant)(thePerPictureGroup->scene_illuminant);

    if (thePerPictureGroup->focal_length_valid)
      if (filePtr->SetImageInfoProperty (PID_FocalLength, TYP_FocalLength, &aProp))
        *aProp = (float)(thePerPictureGroup->focal_length);

    if (thePerPictureGroup->maximum_aperature_value_valid)
      if (filePtr->SetImageInfoProperty (PID_ApertureValue, TYP_ApertureValue, &aProp))
        *aProp = (float)(thePerPictureGroup->maximum_aperature_value);

    if (thePerPictureGroup->flash_valid)
      if (filePtr->SetImageInfoProperty (PID_Flash, TYP_Flash, &aProp))
        *aProp = (unsigned long)(FPXFlash)(thePerPictureGroup->flash);

    if (thePerPictureGroup->flash_energy_valid)
      if (filePtr->SetImageInfoProperty (PID_FlashEnergy, TYP_FlashEnergy, &aProp))
        *aProp = (float)(thePerPictureGroup->flash_energy);

    if (thePerPictureGroup->flash_return_valid)
      if (filePtr->SetImageInfoProperty (PID_FlashReturn, TYP_FlashReturn, &aProp))
        *aProp = (unsigned long)(FPXFlashReturn)(thePerPictureGroup->flash_return);

    if (thePerPictureGroup->back_light_valid)
      if (filePtr->SetImageInfoProperty (PID_BackLight, TYP_BackLight, &aProp))
        *aProp = (unsigned long)(FPXBackLight)(thePerPictureGroup->back_light);

    if (thePerPictureGroup->subject_location_valid)
      if (filePtr->SetImageInfoProperty (PID_SubjectLocation, TYP_SubjectLocation, &aProp))
        *aProp = (FPXRealArray)(thePerPictureGroup->subject_location);

    if (thePerPictureGroup->exposure_index_valid)
      if (filePtr->SetImageInfoProperty (PID_ExposureIndex, TYP_ExposureIndex, &aProp))
        *aProp = (float)(thePerPictureGroup->exposure_index);
        
    if (thePerPictureGroup->special_effects_optical_filter_valid)
      if (filePtr->SetImageInfoProperty (PID_OpticalFilter, TYP_OpticalFilter, &aProp))
        *aProp = (FPXLongArray)(thePerPictureGroup->special_effects_optical_filter);
        

    if (thePerPictureGroup->per_picture_notes_valid)
      if (filePtr->SetImageInfoProperty (PID_PerPictureNotes, TYP_PerPictureNotes, &aProp))
        *aProp = (FPXWideStr)(thePerPictureGroup->per_picture_notes);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetPerPictureGroup (
        FPXImageHandle*                      theFPX,
              FPXPerPictureCameraSettingsGroup*    thePerPictureGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    // capture_date may not be FPXStr 
    if (filePtr->GetImageInfoProperty (PID_CaptureDate, &aProp)) {
      thePerPictureGroup->capture_date = (FILETIME)(*aProp);
      thePerPictureGroup->capture_date_valid = true;
    } else
      thePerPictureGroup->capture_date_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ExposureTime, &aProp)) {
      thePerPictureGroup->exposure_time = (float)(*aProp);
      thePerPictureGroup->exposure_time_valid = true;
    } else
      thePerPictureGroup->exposure_time_valid = false;

    if (filePtr->GetImageInfoProperty (PID_F_Number, &aProp)) {
      thePerPictureGroup->f_number = (float)(*aProp);
      thePerPictureGroup->f_number_valid = true;
    } else
      thePerPictureGroup->f_number_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ExposureProgram, &aProp)) {
      thePerPictureGroup->exposure_program = (FPXExposureProgram)(unsigned long)(*aProp);
      if (thePerPictureGroup->exposure_program > FPX_LANDSCAPE_MODE)
        thePerPictureGroup->exposure_program = FPX_UNIDENTIFIED_EXPOSURE_PROGRAM;
      thePerPictureGroup->exposure_program_valid = true;
    } else
      thePerPictureGroup->exposure_program_valid = false;

    if (filePtr->GetImageInfoProperty (PID_BrightnessValue, &aProp)) {
      thePerPictureGroup->brightness_value = (FPXRealArray)(*aProp);
      thePerPictureGroup->brightness_value_valid = true;
    } else
      thePerPictureGroup->brightness_value_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ExposureBiasValue, &aProp)) {
      thePerPictureGroup->exposure_bias_value = (float)(*aProp);
      thePerPictureGroup->exposure_bias_value_valid = true;
    } else
      thePerPictureGroup->exposure_bias_value_valid = false;

    if (filePtr->GetImageInfoProperty (PID_SubjectDistance, &aProp)) {
      thePerPictureGroup->subject_distance = (FPXRealArray)(*aProp);
      thePerPictureGroup->subject_distance_valid = true;
    } else
      thePerPictureGroup->subject_distance_valid = false;

    if (filePtr->GetImageInfoProperty (PID_MeteringMode, &aProp)) {
      thePerPictureGroup->metering_mode = (FPXMeteringMode)(unsigned long)(*aProp);
      if (thePerPictureGroup->metering_mode > FPX_MULTI_SPOT)
        thePerPictureGroup->metering_mode = FPX_UNIDENTIFED_METERING_MODE;
      thePerPictureGroup->metering_mode_valid = true;
    } else
      thePerPictureGroup->metering_mode_valid = false;

    if (filePtr->GetImageInfoProperty (PID_SceneIlluminant, &aProp)) {
      thePerPictureGroup->scene_illuminant = (FPXSceneIlluminant)(unsigned long)(*aProp);
      if (thePerPictureGroup->scene_illuminant > FPX_D75_ILLUMINANT)
        thePerPictureGroup->scene_illuminant = FPX_UNIDENTIFIED_SCENE_ILLUMINANT;
      thePerPictureGroup->scene_illuminant_valid = true;
    } else
      thePerPictureGroup->scene_illuminant_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FocalLength, &aProp)) {
      thePerPictureGroup->focal_length = (float)(*aProp);
      thePerPictureGroup->focal_length_valid = true;
    } else
      thePerPictureGroup->focal_length_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ApertureValue, &aProp)) {
      thePerPictureGroup->maximum_aperature_value = (float)(*aProp);
      thePerPictureGroup->maximum_aperature_value_valid = true;
    } else
      thePerPictureGroup->maximum_aperature_value_valid = false;

    if (filePtr->GetImageInfoProperty (PID_Flash, &aProp)) {
      thePerPictureGroup->flash = (FPXFlash)(unsigned long)(*aProp);
      if (thePerPictureGroup->flash > FPX_FLASH_USED)
        thePerPictureGroup->flash = FPX_FLASH_USE_UNKNOWN;
      thePerPictureGroup->flash_valid = true;
    } else
      thePerPictureGroup->flash_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FlashEnergy, &aProp)) {
      thePerPictureGroup->flash_energy = (float)(*aProp);
      thePerPictureGroup->flash_energy_valid = true;
    } else
      thePerPictureGroup->flash_energy_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FlashReturn, &aProp)) {
      thePerPictureGroup->flash_return = (FPXFlashReturn)(unsigned long)(*aProp);
      if (thePerPictureGroup->flash_return > FPX_SUBJECT_INSIDE_FLASH_RANGE)
        thePerPictureGroup->flash_return = FPX_NOT_A_CAMERA_FEATURE_FLASH_RETURN;
      thePerPictureGroup->flash_return_valid = true;
    } else
      thePerPictureGroup->flash_return_valid = false;

    if (filePtr->GetImageInfoProperty (PID_BackLight, &aProp)) {
      thePerPictureGroup->back_light = (FPXBackLight)(unsigned long)(*aProp);
      if (thePerPictureGroup->back_light > FPX_BACK_LIT2)
        thePerPictureGroup->back_light = FPX_NOT_A_CAMERA_FEATURE_BACKLIGHT;
      thePerPictureGroup->back_light_valid = true;
    } else
      thePerPictureGroup->back_light_valid = false;

    if (filePtr->GetImageInfoProperty (PID_SubjectLocation, &aProp)) {
      thePerPictureGroup->subject_location = (FPXRealArray)(*aProp);
      thePerPictureGroup->subject_location_valid = true;
    } else
      thePerPictureGroup->subject_location_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ExposureIndex, &aProp)) {
      thePerPictureGroup->exposure_index = (float)(*aProp);
      thePerPictureGroup->exposure_index_valid = true;
    } else
      thePerPictureGroup->exposure_index_valid = false;

    if (filePtr->GetImageInfoProperty (PID_OpticalFilter, &aProp)) {
      thePerPictureGroup->special_effects_optical_filter = (FPXLongArray)(*aProp);
      thePerPictureGroup->special_effects_optical_filter_valid = true;
    } else
      thePerPictureGroup->special_effects_optical_filter_valid = false;

    if (filePtr->GetImageInfoProperty (PID_PerPictureNotes, &aProp)) {
      thePerPictureGroup->per_picture_notes = (FPXWideStr)(*aProp);
      thePerPictureGroup->per_picture_notes_valid = true;
    } else
      thePerPictureGroup->per_picture_notes_valid = false;

  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}
              
//  ------------------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data): Digital Camera Characterization Group
//  ------------------------------------------------------------------------------------

FPXStatus FPX_SetDigitalCameraGroup (
        FPXImageHandle*                        theFPX,
              FPXDigitalCameraCharacterizationGroup* theDigitalCameraGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (theDigitalCameraGroup->sensing_method_valid)
      if (filePtr->SetImageInfoProperty (PID_SensingMethod, TYP_SensingMethod, &aProp))
        *aProp = (unsigned long)(FPXSensingMethod)(theDigitalCameraGroup->sensing_method);

    if (theDigitalCameraGroup->focal_plane_x_resolution_valid)
      if (filePtr->SetImageInfoProperty (PID_FocalXResolution, TYP_FocalXResolution, &aProp))
        *aProp = (float)(theDigitalCameraGroup->focal_plane_x_resolution);

    if (theDigitalCameraGroup->focal_plane_y_resolution_valid)
      if (filePtr->SetImageInfoProperty (PID_FocalYResolution, TYP_FocalYResolution, &aProp))
        *aProp = (float)(theDigitalCameraGroup->focal_plane_y_resolution);

    if (theDigitalCameraGroup->focal_plane_resolution_unit_valid)
      if (filePtr->SetImageInfoProperty (PID_FocalResolutionUnit, TYP_FocalResolutionUnit, &aProp))
        *aProp = (unsigned long)(FPXResolutionUnit)(theDigitalCameraGroup->focal_plane_resolution_unit);
    
    // PTCH_DCG - reinstated Spatial Frequency Response using OLEPROP.CPP conversions
    if (theDigitalCameraGroup->spacial_frequency_valid)
      if (filePtr->SetImageInfoProperty (PID_SpatialFrequencyResponse, TYP_SpatialFrequencyResponse, &aProp)) 
    {
        VECTOR *vec = FPXSpacialFrequencyResponseBlockToVector( theDigitalCameraGroup->spacial_frequency);    
        *aProp = vec;
    }
    
    // PTCH_DCG - reinstated cfa_pattern using OLEPROP.CPP conversions
    if (theDigitalCameraGroup->cfa_pattern_valid)
      if (filePtr->SetImageInfoProperty (PID_CFAPattern, TYP_CFAPattern, &aProp))
    {
        VECTOR *vec = FPXCFA_PatternBlockToVector( theDigitalCameraGroup->cfa_pattern);   
        *aProp = vec;
    }

    if (theDigitalCameraGroup->spectral_sensitivity_valid)
      if (filePtr->SetImageInfoProperty (PID_SpectralSensitivity, TYP_SpectralSensitivity, &aProp))
        *aProp = (FPXWideStr)(theDigitalCameraGroup->spectral_sensitivity);

    if (theDigitalCameraGroup->iso_speed_ratings_valid)
      if (filePtr->SetImageInfoProperty (PID_ISOSpeedRatings, TYP_ISOSpeedRatings, &aProp))
        *aProp = (FPXShortArray)(theDigitalCameraGroup->iso_speed_ratings);

    // PTCH_DCG - reinstated oecf using OLEPROP.CPP conversions
    if (theDigitalCameraGroup->oecf_valid)
      if (filePtr->SetImageInfoProperty (PID_OECF, TYP_OECF, &aProp)) {
        VECTOR *vec = FPXOECF_BlockToVector( theDigitalCameraGroup->oecf);    
        *aProp = vec;
      }
    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetDigitalCameraGroup (
        FPXImageHandle*                        theFPX,
              FPXDigitalCameraCharacterizationGroup* theDigitalCameraGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetImageInfoProperty (PID_SensingMethod, &aProp)) {
      theDigitalCameraGroup->sensing_method = (FPXSensingMethod)(unsigned long)(*aProp);
      if (theDigitalCameraGroup->sensing_method > FPX_COLOR_SEQUENCIAL_LINEAR_SENSOR)
        theDigitalCameraGroup->sensing_method = FPX_UNDEFINED;
      theDigitalCameraGroup->sensing_method_valid = true;
    } else
      theDigitalCameraGroup->sensing_method_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FocalXResolution, &aProp)) {
      theDigitalCameraGroup->focal_plane_x_resolution = (float)(*aProp);
      theDigitalCameraGroup->focal_plane_x_resolution_valid = true;
    } else
      theDigitalCameraGroup->focal_plane_x_resolution_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FocalYResolution, &aProp)) {
      theDigitalCameraGroup->focal_plane_y_resolution = (float)(*aProp);
      theDigitalCameraGroup->focal_plane_y_resolution_valid = true;
    } else
      theDigitalCameraGroup->focal_plane_y_resolution_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FocalResolutionUnit, &aProp)) {
      theDigitalCameraGroup->focal_plane_resolution_unit = (FPXResolutionUnit)(unsigned long)(*aProp);
      theDigitalCameraGroup->focal_plane_resolution_unit_valid = true;
    } else
      theDigitalCameraGroup->focal_plane_resolution_unit_valid = false;

    // PTCH_DCG - reinstated Spatial Frequency Response using OLEPROP.CPP conversions
    if (filePtr->GetImageInfoProperty (PID_SpatialFrequencyResponse, &aProp)) {
      theDigitalCameraGroup->spacial_frequency = (FPXSpacialFrequencyResponseBlock)(*aProp);
      theDigitalCameraGroup->spacial_frequency_valid = true;
    } else
      theDigitalCameraGroup->spacial_frequency_valid = false;

    // PTCH_DCG - reinstated cfa_pattern
    if (filePtr->GetImageInfoProperty (PID_CFAPattern, &aProp)) {
      theDigitalCameraGroup->cfa_pattern = (FPXCFA_PatternBlock)(*aProp);
      theDigitalCameraGroup->cfa_pattern_valid = true;
    } else
      theDigitalCameraGroup->cfa_pattern_valid = false;


    if (filePtr->GetImageInfoProperty (PID_SpectralSensitivity, &aProp)) {
      theDigitalCameraGroup->spectral_sensitivity = (FPXWideStr)(*aProp);
      theDigitalCameraGroup->spectral_sensitivity_valid = true;
    } else
      theDigitalCameraGroup->spectral_sensitivity_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ISOSpeedRatings, &aProp)) {
      theDigitalCameraGroup->iso_speed_ratings = (FPXShortArray)(*aProp);
      theDigitalCameraGroup->iso_speed_ratings_valid = true;
    } else
      theDigitalCameraGroup->iso_speed_ratings_valid = false;

    // PTCH_DCG - reinstated oecf using OLEPROP.CPP conversion
    if (filePtr->GetImageInfoProperty (PID_OECF, &aProp)) {
      theDigitalCameraGroup->oecf = (FPXOECF_Block)(*aProp);
      theDigitalCameraGroup->oecf_valid = true;

    } else
      theDigitalCameraGroup->oecf_valid = false;

  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

//  ------------------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data):  Film Description Group
//  ------------------------------------------------------------------------------------

FPXStatus FPX_SetFilmDescriptionGroup (
        FPXImageHandle*          theFPX,
              FPXFilmDescriptionGroup* theFilmGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (theFilmGroup->film_brand_valid)
      if (filePtr->SetImageInfoProperty (PID_FilmBrand, TYP_FilmBrand, &aProp))
        *aProp = (FPXWideStr)(theFilmGroup->film_brand);

    if (theFilmGroup->film_category_valid)
      if (filePtr->SetImageInfoProperty (PID_FilmCategory, TYP_FilmCategory, &aProp))
        *aProp = (unsigned long)(FPXFilmCategory)(theFilmGroup->film_category);

    if (theFilmGroup->film_size_valid)
      if (filePtr->SetImageInfoProperty (PID_FilmSize, TYP_FilmSize, &aProp)) {
        
        VECTOR vec; 
        vec.cElements = 3; 
        vec.pvar = new VARIANT[vec.cElements]; 
        
        V_VT(&vec.pvar[0]) = VT_R4; 
        V_R4(&vec.pvar[0]) = theFilmGroup->film_size_x; 
        V_VT(&vec.pvar[1]) = VT_R4; 
        V_R4(&vec.pvar[1]) = theFilmGroup->film_size_y; 
        V_VT(&vec.pvar[2]) = VT_UI2; 
        V_I2(&vec.pvar[2]) = (unsigned short)theFilmGroup->film_size_unit; 
        
        *aProp = &vec;
      }
      
    if (theFilmGroup->film_roll_number_valid)
      if (filePtr->SetImageInfoProperty (PID_FilmRollNumber, TYP_FilmRollNumber, &aProp))
        *aProp = (unsigned long)(theFilmGroup->film_roll_number);

    if (theFilmGroup->film_frame_number_valid)
      if (filePtr->SetImageInfoProperty (PID_FilmFrameNumber, TYP_FilmFrameNumber, &aProp))
        *aProp = (unsigned long)(theFilmGroup->film_frame_number);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetFilmDescriptionGroup (
        FPXImageHandle*          theFPX,
              FPXFilmDescriptionGroup* theFilmGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetImageInfoProperty (PID_FilmBrand, &aProp)) {
      theFilmGroup->film_brand = (FPXWideStr)(*aProp);
      theFilmGroup->film_brand_valid = true;
    } else
      theFilmGroup->film_brand_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FilmCategory, &aProp)) {
      theFilmGroup->film_category = (FPXFilmCategory)(unsigned long)(*aProp);
      if (theFilmGroup->film_category > FPX_INTERNEGATIVE_COLOR)
        theFilmGroup->film_category = FPX_UNIDENTIFIED;
      theFilmGroup->film_category_valid = true;
    } else
      theFilmGroup->film_category_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FilmSize, &aProp)) {
      VECTOR * pvec = (VECTOR *)(*aProp); 
      theFilmGroup->film_size_x   = V_R4(&pvec->pvar[0]);
      theFilmGroup->film_size_y   = V_R4(&pvec->pvar[1]);
      theFilmGroup->film_size_unit= (FPXResolutionUnit)V_I2(&pvec->pvar[2]);
      theFilmGroup->film_size_valid = true;
    } else
      theFilmGroup->film_size_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FilmRollNumber, &aProp)) {
      theFilmGroup->film_roll_number = (unsigned long)(*aProp);
      theFilmGroup->film_roll_number_valid = true;
    } else
      theFilmGroup->film_roll_number_valid = false;

    if (filePtr->GetImageInfoProperty (PID_FilmFrameNumber, &aProp)) {
      theFilmGroup->film_frame_number = (unsigned long)(*aProp);
      theFilmGroup->film_frame_number_valid = true;
    } else
      theFilmGroup->film_frame_number_valid = false;

  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

//  ------------------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data):  Original Document Scan Description Group
//  ------------------------------------------------------------------------------------

FPXStatus FPX_SetOriginalDocDescriptionGroup (
        FPXImageHandle*                          theFPX,
              FPXOriginalDocumentScanDescriptionGroup* theDocGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());
      
  if (filePtr) {
    OLEProperty* aProp;
    
    if (theDocGroup->original_scanned_image_size_valid)
      if (filePtr->SetImageInfoProperty (PID_ScannedImageSize, TYP_ScannedImageSize, &aProp)) {
        VECTOR vec;
        vec.cElements = 3;
        vec.pvar = new VARIANT[vec.cElements];

        V_VT(&vec.pvar[0]) = VT_R4;
        V_R4(&vec.pvar[0]) = theDocGroup->original_scanned_image_size.original_size_x;
        V_VT(&vec.pvar[1]) = VT_R4;
        V_R4(&vec.pvar[1]) = theDocGroup->original_scanned_image_size.original_size_y;
        V_VT(&vec.pvar[2]) = VT_UI2;
        V_I2(&vec.pvar[2]) = (unsigned short)theDocGroup->original_scanned_image_size.original_size_unit;
        
        *aProp = &vec;
      }

    if (theDocGroup->original_document_size_valid)
      if (filePtr->SetImageInfoProperty (PID_DocumentSize, TYP_DocumentSize, &aProp)) {
        VECTOR vec;
        vec.cElements = 3;
        vec.pvar = new VARIANT[vec.cElements];

        V_VT(&vec.pvar[0]) = VT_R4;
        V_R4(&vec.pvar[0]) = theDocGroup->original_document_size.original_size_x;
        V_VT(&vec.pvar[1]) = VT_R4;
        V_R4(&vec.pvar[1]) = theDocGroup->original_document_size.original_size_y;
        V_VT(&vec.pvar[2]) = VT_UI2;
        V_I2(&vec.pvar[2]) = (unsigned short)theDocGroup->original_document_size.original_size_unit;
        
        *aProp = &vec;
      }

    if (theDocGroup->original_medium_valid)
      if (filePtr->SetImageInfoProperty (PID_OriginalMedium, TYP_OriginalMedium, &aProp))
        *aProp = (unsigned long)(FPXOriginalMedium)(theDocGroup->original_medium);

    if (theDocGroup->type_of_reflection_original_valid)
      if (filePtr->SetImageInfoProperty (PID_TypeOfOriginal, TYP_TypeOfOriginal, &aProp))
        *aProp = (unsigned long)(FPXTypeOfReflectionOriginal)(theDocGroup->type_of_reflection_original);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetOriginalDocDescriptionGroup (
        FPXImageHandle*                          theFPX,
              FPXOriginalDocumentScanDescriptionGroup* theDocGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());
      
  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetImageInfoProperty (PID_ScannedImageSize, &aProp)) {
      VECTOR * pvec = (VECTOR *)(*aProp);
      theDocGroup->original_scanned_image_size.original_size_x = V_R4(&pvec->pvar[0]);
      theDocGroup->original_scanned_image_size.original_size_y = V_R4(&pvec->pvar[1]);
      theDocGroup->original_scanned_image_size.original_size_unit =
                          (FPXResolutionUnit)V_I2(&pvec->pvar[2]);
      theDocGroup->original_scanned_image_size_valid = true;
    } else
      theDocGroup->original_scanned_image_size_valid = false;

    if (filePtr->GetImageInfoProperty (PID_DocumentSize, &aProp)) {
      VECTOR * pvec = (VECTOR *)(*aProp);   
      theDocGroup->original_document_size.original_size_x = V_R4(&pvec->pvar[0]);
      theDocGroup->original_document_size.original_size_y = V_R4(&pvec->pvar[1]);
      theDocGroup->original_document_size.original_size_unit =
                          (FPXResolutionUnit)V_I2(&pvec->pvar[2]);
      theDocGroup->original_document_size_valid = true;
    } else
      theDocGroup->original_document_size_valid = false;

    if (filePtr->GetImageInfoProperty (PID_OriginalMedium, &aProp)) {
      theDocGroup->original_medium = (FPXOriginalMedium)(unsigned long)(*aProp);
      if (theDocGroup->original_medium > FPX_LINE_ART)
        theDocGroup->original_medium = FPX_UNIDENTIFIED_ORIGINAL_MEDIUM;
      theDocGroup->original_medium_valid = true;
    } else
      theDocGroup->original_medium_valid = false;

    if (filePtr->GetImageInfoProperty (PID_TypeOfOriginal, &aProp)) {
      theDocGroup->type_of_reflection_original = (FPXTypeOfReflectionOriginal)(unsigned long)(*aProp);
      if (theDocGroup->type_of_reflection_original > FPX_COLOR_DOCUMENT)
        theDocGroup->type_of_reflection_original = FPX_UNIDENTIFIED_TYPE_OF_REFLECTION_ORIGINAL;
      theDocGroup->type_of_reflection_original_valid = true;
    } else
      theDocGroup->type_of_reflection_original_valid = false;

  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

//  ------------------------------------------------------------------------------------
//  Image Info Property Set (a.k.a. Non-Image data):  Scan Device Property Group
//  ------------------------------------------------------------------------------------

FPXStatus FPX_SetScanDevicePropertyGroup (
        FPXImageHandle*             theFPX,
              FPXScanDevicePropertyGroup* theScanGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (theScanGroup->scanner_manufacturer_name_valid)
      if (filePtr->SetImageInfoProperty (PID_ScannerManufacturer, TYP_ScannerManufacturer, &aProp))
        *aProp = (FPXWideStr)(theScanGroup->scanner_manufacturer_name);

    if (theScanGroup->scanner_model_name_valid)
      if (filePtr->SetImageInfoProperty (PID_ScannerModel, TYP_ScannerModel, &aProp))
        *aProp = (FPXWideStr)(theScanGroup->scanner_model_name);

    if (theScanGroup->scanner_serial_number_valid)
      if (filePtr->SetImageInfoProperty (PID_ScannerSerialNumber, TYP_ScannerSerialNumber, &aProp))
        *aProp = (FPXWideStr)(theScanGroup->scanner_serial_number);

    if (theScanGroup->scan_software_valid)
      if (filePtr->SetImageInfoProperty (PID_ScanSoftware, TYP_ScanSoftware, &aProp))
        *aProp = (FPXWideStr)(theScanGroup->scan_software);

    if (theScanGroup->scan_software_revision_date_valid)
      if (filePtr->SetImageInfoProperty (PID_ScanSoftRevDate, TYP_ScanSoftRevDate, &aProp))
        *aProp = (DATE)(theScanGroup->scan_software_revision_date);

    if (theScanGroup->service_bureau_org_name_valid)
      if (filePtr->SetImageInfoProperty (PID_ServiceBureau, TYP_ServiceBureau, &aProp))
        *aProp = (FPXWideStr)(theScanGroup->service_bureau_org_name);

    if (theScanGroup->scan_operator_id_valid)
      if (filePtr->SetImageInfoProperty (PID_ScanOperatorID, TYP_ScanOperatorID, &aProp))
        *aProp = (FPXWideStr)(theScanGroup->scan_operator_id);

    if (theScanGroup->scan_date_valid)
      if (filePtr->SetImageInfoProperty (PID_ScanDate, TYP_ScanDate, &aProp))
        *aProp = (FILETIME)(theScanGroup->scan_date);

    if (theScanGroup->last_modified_date_valid)
      if (filePtr->SetImageInfoProperty (PID_LastModifiedDate, TYP_LastModifiedDate, &aProp))
        *aProp = (FILETIME)(theScanGroup->last_modified_date);

    if (theScanGroup->scanner_pixel_size_valid)
      if (filePtr->SetImageInfoProperty (PID_ScannerPixelSize, TYP_ScannerPixelSize, &aProp))
        *aProp = (float)(theScanGroup->scanner_pixel_size);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetScanDevicePropertyGroup (
        FPXImageHandle*             theFPX,
              FPXScanDevicePropertyGroup* theScanGroup)
{
  FPXStatus status = FPX_OK;
  PHierarchicalImage* image =  (PHierarchicalImage*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetImageInfoProperty (PID_ScannerManufacturer, &aProp)) {
      theScanGroup->scanner_manufacturer_name = (FPXWideStr)(*aProp);
      theScanGroup->scanner_manufacturer_name_valid = true;
    } else
      theScanGroup->scanner_manufacturer_name_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ScannerModel, &aProp)) {
      theScanGroup->scanner_model_name = (FPXWideStr)(*aProp);
      theScanGroup->scanner_model_name_valid = true;
    } else
      theScanGroup->scanner_model_name_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ScannerSerialNumber, &aProp)) {
      theScanGroup->scanner_serial_number = (FPXWideStr)(*aProp);
      theScanGroup->scanner_serial_number_valid = true;
    } else
      theScanGroup->scanner_serial_number_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ScanSoftware, &aProp)) {
      theScanGroup->scan_software = (FPXWideStr)(*aProp);
      theScanGroup->scan_software_valid = true;
    } else
      theScanGroup->scan_software_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ScanSoftRevDate, &aProp)) {
      theScanGroup->scan_software_revision_date = (DATE)(*aProp);
      theScanGroup->scan_software_revision_date_valid = true;
    } else
      theScanGroup->scan_software_revision_date_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ServiceBureau, &aProp)) {
      theScanGroup->service_bureau_org_name = (FPXWideStr)(*aProp);
      theScanGroup->service_bureau_org_name_valid = true;
    } else
      theScanGroup->service_bureau_org_name_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ScanOperatorID, &aProp)) {
      theScanGroup->scan_operator_id = (FPXWideStr)(*aProp);
      theScanGroup->scan_operator_id_valid = true;
    } else
      theScanGroup->scan_operator_id_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ScanDate, &aProp)) {
      theScanGroup->scan_date = (FILETIME)(*aProp);
      theScanGroup->scan_date_valid = true;
    } else
      theScanGroup->scan_date_valid = false;

    if (filePtr->GetImageInfoProperty (PID_LastModifiedDate, &aProp)) {
      theScanGroup->last_modified_date = (FILETIME)(*aProp);
      theScanGroup->last_modified_date_valid = true;
    } else
      theScanGroup->last_modified_date_valid = false;

    if (filePtr->GetImageInfoProperty (PID_ScannerPixelSize, &aProp)) {
      theScanGroup->scanner_pixel_size = (float)(*aProp);
      theScanGroup->scanner_pixel_size_valid = true;
    } else
      theScanGroup->scanner_pixel_size_valid = false;

  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}



//  - EOF ----------------------------------------------------------------------
