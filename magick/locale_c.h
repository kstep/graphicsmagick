#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 619

#define MGK_BlobErrorUnableToCreateBlob 1
#define MGK_BlobErrorUnableToOpenFile 2
#define MGK_BlobErrorUnableToReadFile 3
#define MGK_BlobErrorUnableToReadToOffset 4
#define MGK_BlobErrorUnableToSeekToOffset 5
#define MGK_BlobErrorUnableToWriteBlob 6
#define MGK_BlobErrorUnrecognizedImageFormat 7
#define MGK_BlobFatalErrorDefault 8
#define MGK_BlobWarningDefault 9
#define MGK_CacheErrorInconsistentPersistentCacheDepth 10
#define MGK_CacheErrorPixelCacheIsNotOpen 11
#define MGK_CacheErrorUnableToCloneCache 12
#define MGK_CacheErrorUnableToExtendCache 13
#define MGK_CacheErrorUnableToGetCacheNexus 14
#define MGK_CacheErrorUnableToGetPixelsFromCache 15
#define MGK_CacheErrorUnableToOpenCache 16
#define MGK_CacheErrorUnableToPeristPixelCache 17
#define MGK_CacheErrorUnableToReadPixelCache 18
#define MGK_CacheErrorUnableToSyncCache 19
#define MGK_CacheFatalErrorDiskAllocationFailed 20
#define MGK_CacheFatalErrorUnableToExtendPixelCache 21
#define MGK_CacheWarningDefault 22
#define MGK_CoderErrorColormapTypeNotSupported 23
#define MGK_CoderErrorColorspaceModelIsNotSupported 24
#define MGK_CoderErrorColorTypeNotSupported 25
#define MGK_CoderErrorCompressionNotValid 26
#define MGK_CoderErrorDataEncodingSchemeIsNotSupported 27
#define MGK_CoderErrorDataStorageTypeIsNotSupported 28
#define MGK_CoderErrorDeltaPNGNotSupported 29
#define MGK_CoderErrorEncryptedWPGImageFileNotSupported 30
#define MGK_CoderErrorFractalCompressionNotSupported 31
#define MGK_CoderErrorImageColumnOrRowSizeIsNotSupported 32
#define MGK_CoderErrorImageDoesNotHaveAMatteChannel 33
#define MGK_CoderErrorImageIsNotTiled 34
#define MGK_CoderErrorImageTypeNotSupported 35
#define MGK_CoderErrorIncompatibleSizeOfDouble 36
#define MGK_CoderErrorIrregularChannelGeometryNotSupported 37
#define MGK_CoderErrorJNGCompressionNotSupported 38
#define MGK_CoderErrorJPEGCompressionNotSupported 39
#define MGK_CoderErrorJPEGEmbeddingFailed 40
#define MGK_CoderErrorLocationTypeIsNotSupported 41
#define MGK_CoderErrorMapStorageTypeIsNotSupported 42
#define MGK_CoderErrorMSBByteOrderNotSupported 43
#define MGK_CoderErrorMultidimensionalMatricesAreNotSupported 44
#define MGK_CoderErrorMultipleRecordListNotSupported 45
#define MGK_CoderErrorNo8BIMDataIsAvailable 46
#define MGK_CoderErrorNoAPP1DataIsAvailable 47
#define MGK_CoderErrorNoBitmapOnClipboard 48
#define MGK_CoderErrorNoColorProfileAvailable 49
#define MGK_CoderErrorNoDataReturned 50
#define MGK_CoderErrorNoImageVectorGraphics 51
#define MGK_CoderErrorNoIPTCInfoWasFound 52
#define MGK_CoderErrorNoIPTCProfileAvailable 53
#define MGK_CoderErrorNotASupportedImageFile 54
#define MGK_CoderErrorNumberOfImagesIsNotSupported 55
#define MGK_CoderErrorOnlyContinuousTonePictureSupported 56
#define MGK_CoderErrorOnlyLevelZerofilesSupported 57
#define MGK_CoderErrorPNGCompressionNotSupported 58
#define MGK_CoderErrorPNGLibraryTooOld 59
#define MGK_CoderErrorRLECompressionNotSupported 60
#define MGK_CoderErrorUnableToCopyProfile 61
#define MGK_CoderErrorUnableToCreateADC 62
#define MGK_CoderErrorUnableToCreateBitmap 63
#define MGK_CoderErrorUnableToDecompressImage 64
#define MGK_CoderErrorUnableToInitializeFPXLibrary 65
#define MGK_CoderErrorUnableToOpenBlob 66
#define MGK_CoderErrorUnableToReadAspectRatio 67
#define MGK_CoderErrorUnableToReadCIELABImages 68
#define MGK_CoderErrorUnableToReadSummaryInfo 69
#define MGK_CoderErrorUnableToSetAffineMatrix 70
#define MGK_CoderErrorUnableToSetAspectRatio 71
#define MGK_CoderErrorUnableToSetColorTwist 72
#define MGK_CoderErrorUnableToSetContrast 73
#define MGK_CoderErrorUnableToSetFilteringValue 74
#define MGK_CoderErrorUnableToSetImageComments 75
#define MGK_CoderErrorUnableToSetImageTitle 76
#define MGK_CoderErrorUnableToSetJPEGLevel 77
#define MGK_CoderErrorUnableToSetRegionOfInterest 78
#define MGK_CoderErrorUnableToSetSummaryInfo 79
#define MGK_CoderErrorUnableToTranslateText 80
#define MGK_CoderErrorUnableToWriteMPEGParameters 81
#define MGK_CoderErrorUnableToZipCompressImage 82
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 83
#define MGK_CoderErrorZipCompressionNotSupported 84
#define MGK_CoderFatalErrorDefault 85
#define MGK_CoderWarningLosslessToLossyJPEGConversion 86
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 87
#define MGK_ConfigureErrorRegistryKeyLookupFailed 88
#define MGK_ConfigureErrorUnableToAccessConfigureFile 89
#define MGK_ConfigureErrorUnableToAccessFontFile 90
#define MGK_ConfigureErrorUnableToAccessLogFile 91
#define MGK_ConfigureErrorUnableToAccessModuleFile 92
#define MGK_ConfigureFatalErrorDefault 93
#define MGK_ConfigureWarningDefault 94
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 95
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 96
#define MGK_CorruptImageErrorColormapExceeds256Colors 97
#define MGK_CorruptImageErrorCorruptImage 98
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 99
#define MGK_CorruptImageErrorImageTypeNotSupported 100
#define MGK_CorruptImageErrorImproperImageHeader 101
#define MGK_CorruptImageErrorInsufficientImageDataInFile 102
#define MGK_CorruptImageErrorInvalidColormapIndex 103
#define MGK_CorruptImageErrorInvalidFileFormatVersion 104
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 105
#define MGK_CorruptImageErrorMissingImageChannel 106
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 107
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 108
#define MGK_CorruptImageErrorNotABMPImageFile 109
#define MGK_CorruptImageErrorNotACUTImageFile 110
#define MGK_CorruptImageErrorNotADCMImageFile 111
#define MGK_CorruptImageErrorNotADCXImageFile 112
#define MGK_CorruptImageErrorNotADIBImageFile 113
#define MGK_CorruptImageErrorNotADPXImageFile 114
#define MGK_CorruptImageErrorNotAGIFImageFile 115
#define MGK_CorruptImageErrorNotAJNGImageFile 116
#define MGK_CorruptImageErrorNotAMATLABImageFile 117
#define MGK_CorruptImageErrorNotAMNGImageFile 118
#define MGK_CorruptImageErrorNotAMTVImageFile 119
#define MGK_CorruptImageErrorNotAnARTImageFile 120
#define MGK_CorruptImageErrorNotAnAVIImageFile 121
#define MGK_CorruptImageErrorNotAnAVSImageFile 122
#define MGK_CorruptImageErrorNotAnEMFImageFile 123
#define MGK_CorruptImageErrorNotAnICOImageFile 124
#define MGK_CorruptImageErrorNotAOTBImageFile 125
#define MGK_CorruptImageErrorNotAPCDImageFile 126
#define MGK_CorruptImageErrorNotAPCXImageFile 127
#define MGK_CorruptImageErrorNotAPDBImageFile 128
#define MGK_CorruptImageErrorNotAPICTImageFile 129
#define MGK_CorruptImageErrorNotAPIXImageFile 130
#define MGK_CorruptImageErrorNotAPNGImageFile 131
#define MGK_CorruptImageErrorNotAPNMImageFile 132
#define MGK_CorruptImageErrorNotAPSDImageFile 133
#define MGK_CorruptImageErrorNotAPWPImageFile 134
#define MGK_CorruptImageErrorNotARLEImageFile 135
#define MGK_CorruptImageErrorNotASCTImageFile 136
#define MGK_CorruptImageErrorNotASFWImageFile 137
#define MGK_CorruptImageErrorNotASGIImageFile 138
#define MGK_CorruptImageErrorNotASUNImageFile 139
#define MGK_CorruptImageErrorNotATGAImageFile 140
#define MGK_CorruptImageErrorNotATIMImageFile 141
#define MGK_CorruptImageErrorNotATOPOLImageFile 142
#define MGK_CorruptImageErrorNotAVIFFImageFile 143
#define MGK_CorruptImageErrorNotAWBMPImageFile 144
#define MGK_CorruptImageErrorNotAWPGImageFile 145
#define MGK_CorruptImageErrorNotAXBMImageFile 146
#define MGK_CorruptImageErrorNotAXCFImageFile 147
#define MGK_CorruptImageErrorNotAXPMImageFile 148
#define MGK_CorruptImageErrorNotEnoughTiles 149
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 150
#define MGK_CorruptImageErrorTooMuchImageDataInFile 151
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 152
#define MGK_CorruptImageErrorUnableToReadColorProfile 153
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 154
#define MGK_CorruptImageErrorUnableToReadGenericProfile 155
#define MGK_CorruptImageErrorUnableToReadImageData 156
#define MGK_CorruptImageErrorUnableToReadImageHeader 157
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 158
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 159
#define MGK_CorruptImageErrorUnableToReadSubImageData 160
#define MGK_CorruptImageErrorUnableToReadVIDImage 161
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 162
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 163
#define MGK_CorruptImageErrorUnableToUncompressImage 164
#define MGK_CorruptImageErrorUnexpectedEndOfFile 165
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 166
#define MGK_CorruptImageErrorUnknownPatternType 167
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 168
#define MGK_CorruptImageErrorUnrecognizedImageCompression 169
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 170
#define MGK_CorruptImageFatalErrorUnableToPersistKey 171
#define MGK_CorruptImageWarningCompressionNotValid 172
#define MGK_CorruptImageWarningImproperImageHeader 173
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 174
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 175
#define MGK_CorruptImageWarningSkipToSyncByte 176
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 177
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 178
#define MGK_CorruptImageWarningUnrecognizedImageCompression 179
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 180
#define MGK_DelegateErrorDelegateFailed 181
#define MGK_DelegateErrorFailedToComputeOutputSize 182
#define MGK_DelegateErrorFailedToRenderFile 183
#define MGK_DelegateErrorFailedToScanFile 184
#define MGK_DelegateErrorNoTagFound 185
#define MGK_DelegateErrorPostscriptDelegateFailed 186
#define MGK_DelegateErrorUnableToCreateImage 187
#define MGK_DelegateErrorUnableToCreateImageComponent 188
#define MGK_DelegateErrorUnableToDecodeImageFile 189
#define MGK_DelegateErrorUnableToEncodeImageFile 190
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 191
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 192
#define MGK_DelegateErrorUnableToManageJP2Stream 193
#define MGK_DelegateErrorUnableToWriteSVGFormat 194
#define MGK_DelegateFatalErrorDefault 195
#define MGK_DelegateWarningDefault 196
#define MGK_DrawErrorAlreadyPushingPatternDefinition 197
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 198
#define MGK_DrawErrorUnableToPrint 199
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 200
#define MGK_DrawFatalErrorDefault 201
#define MGK_DrawWarningNotARelativeURL 202
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 203
#define MGK_DrawWarningURLNotFound 204
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 205
#define MGK_FileOpenErrorUnableToOpenFile 206
#define MGK_FileOpenErrorUnableToWriteFile 207
#define MGK_FileOpenFatalErrorDefault 208
#define MGK_FileOpenWarningDefault 209
#define MGK_ImageErrorAngleIsDiscontinuous 210
#define MGK_ImageErrorColorspaceColorProfileMismatch 211
#define MGK_ImageErrorImageColorspaceDiffers 212
#define MGK_ImageErrorImageOpacityDiffers 213
#define MGK_ImageErrorImageSequenceIsRequired 214
#define MGK_ImageErrorImageSizeDiffers 215
#define MGK_ImageErrorInvalidColormapIndex 216
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 217
#define MGK_ImageErrorNoImagesWereFound 218
#define MGK_ImageErrorNoImagesWereLoaded 219
#define MGK_ImageErrorNoLocaleImageAttribute 220
#define MGK_ImageErrorTooManyClusters 221
#define MGK_ImageErrorUnableToAppendImage 222
#define MGK_ImageErrorUnableToAssignProfile 223
#define MGK_ImageErrorUnableToAverageImage 224
#define MGK_ImageErrorUnableToCoalesceImage 225
#define MGK_ImageErrorUnableToCompareImages 226
#define MGK_ImageErrorUnableToCreateImageMosaic 227
#define MGK_ImageErrorUnableToCreateStereoImage 228
#define MGK_ImageErrorUnableToDeconstructImageSequence 229
#define MGK_ImageErrorUnableToFlattenImage 230
#define MGK_ImageErrorUnableToResizeImage 231
#define MGK_ImageErrorUnableToSegmentImage 232
#define MGK_ImageErrorUnableToSetClipMask 233
#define MGK_ImageErrorUnableToShearImage 234
#define MGK_ImageErrorWidthOrHeightExceedsLimit 235
#define MGK_ImageFatalErrorUnableToPersistKey 236
#define MGK_ImageWarningDefault 237
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 238
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 239
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 240
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 241
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 242
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 243
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 244
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 245
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 246
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 247
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 248
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 249
#define MGK_MissingDelegateFatalErrorDefault 250
#define MGK_MissingDelegateWarningDefault 251
#define MGK_ModuleErrorFailedToCloseModule 252
#define MGK_ModuleErrorFailedToFindSymbol 253
#define MGK_ModuleErrorUnableToLoadModule 254
#define MGK_ModuleErrorUnableToRegisterImageFormat 255
#define MGK_ModuleErrorUnrecognizedModule 256
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 257
#define MGK_ModuleWarningDefault 258
#define MGK_OptionErrorBevelWidthIsNegative 259
#define MGK_OptionErrorColorSeparatedImageRequired 260
#define MGK_OptionErrorFrameIsLessThanImageSize 261
#define MGK_OptionErrorGeometryDimensionsAreZero 262
#define MGK_OptionErrorGeometryDoesNotContainImage 263
#define MGK_OptionErrorImagesAreNotTheSameSize 264
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 265
#define MGK_OptionErrorImageSmallerThanKernelWidth 266
#define MGK_OptionErrorImageSmallerThanRadius 267
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 268
#define MGK_OptionErrorInputImagesAlreadySpecified 269
#define MGK_OptionErrorKernelRadiusIsTooSmall 270
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 271
#define MGK_OptionErrorMissingAffineMatrix 272
#define MGK_OptionErrorMissingAmount 273
#define MGK_OptionErrorMissingAmplitude 274
#define MGK_OptionErrorMissingAnImageFilename 275
#define MGK_OptionErrorMissingArgument 276
#define MGK_OptionErrorMissingAzimuth 277
#define MGK_OptionErrorMissingBackgroundColor 278
#define MGK_OptionErrorMissingBevelWidth 279
#define MGK_OptionErrorMissingBorderColor 280
#define MGK_OptionErrorMissingClippingPathName 281
#define MGK_OptionErrorMissingCoefficients 282
#define MGK_OptionErrorMissingColorizeValue 283
#define MGK_OptionErrorMissingColors 284
#define MGK_OptionErrorMissingComment 285
#define MGK_OptionErrorMissingCompressionQuality 286
#define MGK_OptionErrorMissingCycleAmount 287
#define MGK_OptionErrorMissingDelay 288
#define MGK_OptionErrorMissingDisposeMethod 289
#define MGK_OptionErrorMissingDissolveValue 290
#define MGK_OptionErrorMissingDistance 291
#define MGK_OptionErrorMissingDrawingPrimitive 292
#define MGK_OptionErrorMissingEnodingType 293
#define MGK_OptionErrorMissingEventMask 294
#define MGK_OptionErrorMissingFilename 295
#define MGK_OptionErrorMissingFillColor 296
#define MGK_OptionErrorMissingFontName 297
#define MGK_OptionErrorMissingFormatSpecifier 298
#define MGK_OptionErrorMissingGeometry 299
#define MGK_OptionErrorMissingImageDepth 300
#define MGK_OptionErrorMissingImageFrames 301
#define MGK_OptionErrorMissingImageGamma 302
#define MGK_OptionErrorMissingImageIterations 303
#define MGK_OptionErrorMissingImageLabel 304
#define MGK_OptionErrorMissingImageProfile 305
#define MGK_OptionErrorMissingImageRotation 306
#define MGK_OptionErrorMissingImageScene 307
#define MGK_OptionErrorMissingImageType 308
#define MGK_OptionErrorMissingImplodeAmount 309
#define MGK_OptionErrorMissingListName 310
#define MGK_OptionErrorMissingLogFormat 311
#define MGK_OptionErrorMissingMatrix 312
#define MGK_OptionErrorMissingMatteColor 313
#define MGK_OptionErrorMissingOffset 314
#define MGK_OptionErrorMissingOpaqueColor 315
#define MGK_OptionErrorMissingOrder 316
#define MGK_OptionErrorMissingPageGeometry 317
#define MGK_OptionErrorMissingPaintRadius 318
#define MGK_OptionErrorMissingPassword 319
#define MGK_OptionErrorMissingPoint 320
#define MGK_OptionErrorMissingPointsize 321
#define MGK_OptionErrorMissingResourceLimit 322
#define MGK_OptionErrorMissingResourceType 323
#define MGK_OptionErrorMissingSeedValue 324
#define MGK_OptionErrorMissingServerName 325
#define MGK_OptionErrorMissingSpreadAmount 326
#define MGK_OptionErrorMissingThreshold 327
#define MGK_OptionErrorMissingTile 328
#define MGK_OptionErrorMissingTitle 329
#define MGK_OptionErrorMissingTransparentColor 330
#define MGK_OptionErrorMissingTreeDepth 331
#define MGK_OptionErrorMissingType 332
#define MGK_OptionErrorMissingViewTransform 333
#define MGK_OptionErrorMissingVirtualPixelMethod 334
#define MGK_OptionErrorMissingWidth 335
#define MGK_OptionErrorMustSpecifyAnImageName 336
#define MGK_OptionErrorMustSpecifyImageSize 337
#define MGK_OptionErrorNoBlobDefined 338
#define MGK_OptionErrorNoImagesDefined 339
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 340
#define MGK_OptionErrorNoProfileNameWasGiven 341
#define MGK_OptionErrorNullBlobArgument 342
#define MGK_OptionErrorReferenceImageRequired 343
#define MGK_OptionErrorReferenceIsNotMyType 344
#define MGK_OptionErrorSteganoImageRequired 345
#define MGK_OptionErrorStereoImageRequired 346
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 347
#define MGK_OptionErrorUnableToAddOrRemoveProfile 348
#define MGK_OptionErrorUnableToAllocateICCProfile 349
#define MGK_OptionErrorUnableToAverageImageSequence 350
#define MGK_OptionErrorUnableToBlurImage 351
#define MGK_OptionErrorUnableToChopImage 352
#define MGK_OptionErrorUnableToConstituteImage 353
#define MGK_OptionErrorUnableToConvolveImage 354
#define MGK_OptionErrorUnableToEdgeImage 355
#define MGK_OptionErrorUnableToEqualizeImage 356
#define MGK_OptionErrorUnableToFilterImage 357
#define MGK_OptionErrorUnableToFormatImageMetadata 358
#define MGK_OptionErrorUnableToFrameImage 359
#define MGK_OptionErrorUnableToOilPaintImage 360
#define MGK_OptionErrorUnableToPaintImage 361
#define MGK_OptionErrorUnableToRaiseImage 362
#define MGK_OptionErrorUnableToSharpenImage 363
#define MGK_OptionErrorUnableToThresholdImage 364
#define MGK_OptionErrorUnableToWaveImage 365
#define MGK_OptionErrorUnrecognizedAttribute 366
#define MGK_OptionErrorUnrecognizedChannelType 367
#define MGK_OptionErrorUnrecognizedColor 368
#define MGK_OptionErrorUnrecognizedColormapType 369
#define MGK_OptionErrorUnrecognizedColorspace 370
#define MGK_OptionErrorUnrecognizedComposeOperator 371
#define MGK_OptionErrorUnrecognizedDisposeMethod 372
#define MGK_OptionErrorUnrecognizedElement 373
#define MGK_OptionErrorUnrecognizedEndianType 374
#define MGK_OptionErrorUnrecognizedGravityType 375
#define MGK_OptionErrorUnrecognizedImageCompression 376
#define MGK_OptionErrorUnrecognizedImageFilter 377
#define MGK_OptionErrorUnrecognizedImageFormat 378
#define MGK_OptionErrorUnrecognizedImageMode 379
#define MGK_OptionErrorUnrecognizedImageType 380
#define MGK_OptionErrorUnrecognizedIntentType 381
#define MGK_OptionErrorUnrecognizedInterlaceType 382
#define MGK_OptionErrorUnrecognizedListType 383
#define MGK_OptionErrorUnrecognizedModeType 384
#define MGK_OptionErrorUnrecognizedNoiseType 385
#define MGK_OptionErrorUnrecognizedOption 386
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 387
#define MGK_OptionErrorUnrecognizedPixelMap 388
#define MGK_OptionErrorUnrecognizedPreviewType 389
#define MGK_OptionErrorUnrecognizedResourceType 390
#define MGK_OptionErrorUnrecognizedType 391
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 392
#define MGK_OptionErrorUsageError 393
#define MGK_OptionFatalErrorDeprecatedOption 394
#define MGK_OptionFatalErrorInvalidColorspaceType 395
#define MGK_OptionFatalErrorInvalidEndianType 396
#define MGK_OptionFatalErrorInvalidImageType 397
#define MGK_OptionFatalErrorInvalidInterlaceType 398
#define MGK_OptionFatalErrorMissingAnImageFilename 399
#define MGK_OptionFatalErrorMissingBevelWidth 400
#define MGK_OptionFatalErrorMissingBorderColor 401
#define MGK_OptionFatalErrorMissingColor 402
#define MGK_OptionFatalErrorMissingColors 403
#define MGK_OptionFatalErrorMissingCommand 404
#define MGK_OptionFatalErrorMissingComment 405
#define MGK_OptionFatalErrorMissingCompressionQuality 406
#define MGK_OptionFatalErrorMissingDegrees 407
#define MGK_OptionFatalErrorMissingDepth 408
#define MGK_OptionFatalErrorMissingDisposeMethod 409
#define MGK_OptionFatalErrorMissingEncodingType 410
#define MGK_OptionFatalErrorMissingEventMask 411
#define MGK_OptionFatalErrorMissingFactor 412
#define MGK_OptionFatalErrorMissingFilename 413
#define MGK_OptionFatalErrorMissingFontName 414
#define MGK_OptionFatalErrorMissingForeground 415
#define MGK_OptionFatalErrorMissingGeometry 416
#define MGK_OptionFatalErrorMissingIDNameOrRoot 417
#define MGK_OptionFatalErrorMissingImageDepth 418
#define MGK_OptionFatalErrorMissingLabelName 419
#define MGK_OptionFatalErrorMissingLevel 420
#define MGK_OptionFatalErrorMissingLogFormat 421
#define MGK_OptionFatalErrorMissingMapType 422
#define MGK_OptionFatalErrorMissingMethod 423
#define MGK_OptionFatalErrorMissingName 424
#define MGK_OptionFatalErrorMissingPageGeometry 425
#define MGK_OptionFatalErrorMissingPassword 426
#define MGK_OptionFatalErrorMissingQuality 427
#define MGK_OptionFatalErrorMissingResourceLimit 428
#define MGK_OptionFatalErrorMissingResourceType 429
#define MGK_OptionFatalErrorMissingSceneNumber 430
#define MGK_OptionFatalErrorMissingSeconds 431
#define MGK_OptionFatalErrorMissingServerName 432
#define MGK_OptionFatalErrorMissingSize 433
#define MGK_OptionFatalErrorMissingSnaps 434
#define MGK_OptionFatalErrorMissingThreshold 435
#define MGK_OptionFatalErrorMissingTitle 436
#define MGK_OptionFatalErrorMissingType 437
#define MGK_OptionFatalErrorMissingValue 438
#define MGK_OptionFatalErrorMissingVirtualPixelMethod 439
#define MGK_OptionFatalErrorMissingVisualClass 440
#define MGK_OptionFatalErrorMissingWidth 441
#define MGK_OptionFatalErrorNoImagesWereLoaded 442
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 443
#define MGK_OptionFatalErrorUnableToOpenXServer 444
#define MGK_OptionFatalErrorUnableToPersistKey 445
#define MGK_OptionFatalErrorUnrecognizedColormapType 446
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 447
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 448
#define MGK_OptionFatalErrorUnrecognizedEndianType 449
#define MGK_OptionFatalErrorUnrecognizedFilterType 450
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 451
#define MGK_OptionFatalErrorUnrecognizedImageType 452
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 453
#define MGK_OptionFatalErrorUnrecognizedOption 454
#define MGK_OptionFatalErrorUnrecognizedResourceType 455
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 456
#define MGK_OptionWarningUnrecognizedColor 457
#define MGK_RegistryErrorImageExpected 458
#define MGK_RegistryErrorImageInfoExpected 459
#define MGK_RegistryErrorStructureSizeMismatch 460
#define MGK_RegistryErrorUnableToGetRegistryID 461
#define MGK_RegistryErrorUnableToLocateImage 462
#define MGK_RegistryErrorUnableToSetRegistry 463
#define MGK_RegistryFatalErrorDefault 464
#define MGK_RegistryWarningDefault 465
#define MGK_ResourceLimitErrorCacheResourcesExhausted 466
#define MGK_ResourceLimitErrorMemoryAllocationFailed 467
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 468
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 469
#define MGK_ResourceLimitErrorUnableToAddColorProfile 470
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 471
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 472
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 473
#define MGK_ResourceLimitErrorUnableToAllocateColormap 474
#define MGK_ResourceLimitErrorUnableToAllocateString 475
#define MGK_ResourceLimitErrorUnableToAnnotateImage 476
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 477
#define MGK_ResourceLimitErrorUnableToCloneImage 478
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 479
#define MGK_ResourceLimitErrorUnableToConstituteImage 480
#define MGK_ResourceLimitErrorUnableToConvertFont 481
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 482
#define MGK_ResourceLimitErrorUnableToCreateColormap 483
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 484
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 485
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 486
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 487
#define MGK_ResourceLimitErrorUnableToCreateXWindow 488
#define MGK_ResourceLimitErrorUnableToCropImage 489
#define MGK_ResourceLimitErrorUnableToDespeckleImage 490
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 491
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 492
#define MGK_ResourceLimitErrorUnableToDitherImage 493
#define MGK_ResourceLimitErrorUnableToDrawOnImage 494
#define MGK_ResourceLimitErrorUnableToEdgeImage 495
#define MGK_ResourceLimitErrorUnableToEmbossImage 496
#define MGK_ResourceLimitErrorUnableToEnhanceImage 497
#define MGK_ResourceLimitErrorUnableToFloodfillImage 498
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 499
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 500
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 501
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 502
#define MGK_ResourceLimitErrorUnableToLevelImage 503
#define MGK_ResourceLimitErrorUnableToMagnifyImage 504
#define MGK_ResourceLimitErrorUnableToManageColor 505
#define MGK_ResourceLimitErrorUnableToMapImage 506
#define MGK_ResourceLimitErrorUnableToMapImageSequence 507
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 508
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 509
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 510
#define MGK_ResourceLimitErrorUnableToNormalizeImage 511
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 512
#define MGK_ResourceLimitErrorUnableToQuantizeImage 513
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 514
#define MGK_ResourceLimitErrorUnableToReadTextChunk 515
#define MGK_ResourceLimitErrorUnableToReadXImage 516
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 517
#define MGK_ResourceLimitErrorUnableToResizeImage 518
#define MGK_ResourceLimitErrorUnableToRotateImage 519
#define MGK_ResourceLimitErrorUnableToSampleImage 520
#define MGK_ResourceLimitErrorUnableToScaleImage 521
#define MGK_ResourceLimitErrorUnableToSelectImage 522
#define MGK_ResourceLimitErrorUnableToSharpenImage 523
#define MGK_ResourceLimitErrorUnableToShaveImage 524
#define MGK_ResourceLimitErrorUnableToShearImage 525
#define MGK_ResourceLimitErrorUnableToSortImageColormap 526
#define MGK_ResourceLimitErrorUnableToThresholdImage 527
#define MGK_ResourceLimitErrorUnableToTransformColorspace 528
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 529
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 530
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 531
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 532
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 533
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 534
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 535
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 536
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 537
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 538
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 539
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 540
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 541
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 542
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 543
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 544
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 545
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 546
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 547
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 548
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 549
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 550
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 551
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 552
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 553
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 554
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 555
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 556
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 557
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 558
#define MGK_ResourceLimitFatalErrorUnableToConvertText 559
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 560
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 561
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 562
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 563
#define MGK_ResourceLimitWarningMemoryAllocationFailed 564
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 565
#define MGK_StreamErrorNoStreamHandlerIsDefined 566
#define MGK_StreamErrorPixelCacheIsNotOpen 567
#define MGK_StreamErrorUnableToAcquirePixelStream 568
#define MGK_StreamErrorUnableToSetPixelStream 569
#define MGK_StreamErrorUnableToSyncPixelStream 570
#define MGK_StreamFatalErrorDefault 571
#define MGK_StreamWarningDefault 572
#define MGK_TypeErrorFontSubstitutionRequired 573
#define MGK_TypeErrorUnableToGetTypeMetrics 574
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 575
#define MGK_TypeErrorUnableToReadFont 576
#define MGK_TypeErrorUnrecognizedFontEncoding 577
#define MGK_TypeFatalErrorDefault 578
#define MGK_TypeWarningDefault 579
#define MGK_XServerErrorColorIsNotKnownToServer 580
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 581
#define MGK_XServerErrorStandardColormapIsNotInitialized 582
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 583
#define MGK_XServerErrorUnableToCreateBitmap 584
#define MGK_XServerErrorUnableToCreateColormap 585
#define MGK_XServerErrorUnableToCreatePixmap 586
#define MGK_XServerErrorUnableToCreateProperty 587
#define MGK_XServerErrorUnableToCreateStandardColormap 588
#define MGK_XServerErrorUnableToDisplayImageInfo 589
#define MGK_XServerErrorUnableToGetProperty 590
#define MGK_XServerErrorUnableToGetStandardColormap 591
#define MGK_XServerErrorUnableToGetVisual 592
#define MGK_XServerErrorUnableToGrabMouse 593
#define MGK_XServerErrorUnableToLoadFont 594
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 595
#define MGK_XServerErrorUnableToOpenXServer 596
#define MGK_XServerErrorUnableToReadXAttributes 597
#define MGK_XServerErrorUnableToReadXWindowImage 598
#define MGK_XServerErrorUnrecognizedColormapType 599
#define MGK_XServerErrorUnrecognizedGravityType 600
#define MGK_XServerErrorUnrecognizedVisualSpecifier 601
#define MGK_XServerFatalErrorUnableToAllocateXHints 602
#define MGK_XServerFatalErrorUnableToCreateCursor 603
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 604
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 605
#define MGK_XServerFatalErrorUnableToCreateTextProperty 606
#define MGK_XServerFatalErrorUnableToCreateXImage 607
#define MGK_XServerFatalErrorUnableToCreateXPixmap 608
#define MGK_XServerFatalErrorUnableToCreateXWindow 609
#define MGK_XServerFatalErrorUnableToDisplayImage 610
#define MGK_XServerFatalErrorUnableToDitherImage 611
#define MGK_XServerFatalErrorUnableToGetPixelInfo 612
#define MGK_XServerFatalErrorUnableToGetVisual 613
#define MGK_XServerFatalErrorUnableToLoadFont 614
#define MGK_XServerFatalErrorUnableToMakeXWindow 615
#define MGK_XServerFatalErrorUnableToOpenXServer 616
#define MGK_XServerFatalErrorUnableToViewFonts 617
#define MGK_XServerWarningUnableToGetVisual 618
#define MGK_XServerWarningUsingDefaultVisual 619

#endif

#if defined(_INCLUDE_CATEGORYMAP_TABLE_)
typedef struct _CategoryInfo{
  char *name;
  int offset;
} CategoryInfo;

static const CategoryInfo category_map[] =
  {
    { "Blob", 0 },
    { "Cache", 3 },
    { "Coder", 6 },
    { "Configure", 9 },
    { "Corrupt/Image", 12 },
    { "Delegate", 15 },
    { "Draw", 18 },
    { "File/Open", 21 },
    { "Image", 24 },
    { "Missing/Delegate", 27 },
    { "Module", 30 },
    { "Option", 33 },
    { "Registry", 36 },
    { "Resource/Limit", 39 },
    { "Stream", 42 },
    { "Type", 45 },
    { "XServer", 48 },
    { 0, 50 }
  };
#endif

#if defined(_INCLUDE_SEVERITYMAP_TABLE_)
typedef struct _SeverityInfo{
  char *name;
  int offset;
  ExceptionType severityid;
} SeverityInfo;

static const SeverityInfo severity_map[] =
  {
    { "Blob/Error", 0, BlobError },
    { "Blob/FatalError", 7, BlobFatalError },
    { "Blob/Warning", 8, BlobWarning },
    { "Cache/Error", 9, CacheError },
    { "Cache/FatalError", 19, CacheFatalError },
    { "Cache/Warning", 21, CacheWarning },
    { "Coder/Error", 22, CoderError },
    { "Coder/FatalError", 84, CoderFatalError },
    { "Coder/Warning", 85, CoderWarning },
    { "Configure/Error", 86, ConfigureError },
    { "Configure/FatalError", 92, ConfigureFatalError },
    { "Configure/Warning", 93, ConfigureWarning },
    { "Corrupt/Image/Error", 94, CorruptImageError },
    { "Corrupt/Image/FatalError", 170, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 171, CorruptImageWarning },
    { "Delegate/Error", 180, DelegateError },
    { "Delegate/FatalError", 194, DelegateFatalError },
    { "Delegate/Warning", 195, DelegateWarning },
    { "Draw/Error", 196, DrawError },
    { "Draw/FatalError", 200, DrawFatalError },
    { "Draw/Warning", 201, DrawWarning },
    { "File/Open/Error", 204, FileOpenError },
    { "File/Open/FatalError", 207, FileOpenFatalError },
    { "File/Open/Warning", 208, FileOpenWarning },
    { "Image/Error", 209, ImageError },
    { "Image/FatalError", 235, ImageFatalError },
    { "Image/Warning", 236, ImageWarning },
    { "Missing/Delegate/Error", 237, MissingDelegateError },
    { "Missing/Delegate/FatalError", 249, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 250, MissingDelegateWarning },
    { "Module/Error", 251, ModuleError },
    { "Module/FatalError", 256, ModuleFatalError },
    { "Module/Warning", 257, ModuleWarning },
    { "Option/Error", 258, OptionError },
    { "Option/FatalError", 393, OptionFatalError },
    { "Option/Warning", 456, OptionWarning },
    { "Registry/Error", 457, RegistryError },
    { "Registry/FatalError", 463, RegistryFatalError },
    { "Registry/Warning", 464, RegistryWarning },
    { "Resource/Limit/Error", 465, ResourceLimitError },
    { "Resource/Limit/FatalError", 528, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 563, ResourceLimitWarning },
    { "Stream/Error", 564, StreamError },
    { "Stream/FatalError", 570, StreamFatalError },
    { "Stream/Warning", 571, StreamWarning },
    { "Type/Error", 572, TypeError },
    { "Type/FatalError", 577, TypeFatalError },
    { "Type/Warning", 578, TypeWarning },
    { "XServer/Error", 579, XServerError },
    { "XServer/FatalError", 601, XServerFatalError },
    { "XServer/Warning", 617, XServerWarning },
    { 0, 619, 0 }
  };
#endif

#if defined(_INCLUDE_TAGMAP_TABLE_)
typedef struct _MessageInfo
{
  char *name;
  int messageid;
} MessageInfo;

static const MessageInfo message_map[] =
  {
    { "UnableToCreateBlob", 1 },
    { "UnableToOpenFile", 2 },
    { "UnableToReadFile", 3 },
    { "UnableToReadToOffset", 4 },
    { "UnableToSeekToOffset", 5 },
    { "UnableToWriteBlob", 6 },
    { "UnrecognizedImageFormat", 7 },
    { "Default", 8 },
    { "Default", 9 },
    { "InconsistentPersistentCacheDepth", 10 },
    { "PixelCacheIsNotOpen", 11 },
    { "UnableToCloneCache", 12 },
    { "UnableToExtendCache", 13 },
    { "UnableToGetCacheNexus", 14 },
    { "UnableToGetPixelsFromCache", 15 },
    { "UnableToOpenCache", 16 },
    { "UnableToPeristPixelCache", 17 },
    { "UnableToReadPixelCache", 18 },
    { "UnableToSyncCache", 19 },
    { "DiskAllocationFailed", 20 },
    { "UnableToExtendPixelCache", 21 },
    { "Default", 22 },
    { "ColormapTypeNotSupported", 23 },
    { "ColorspaceModelIsNotSupported", 24 },
    { "ColorTypeNotSupported", 25 },
    { "CompressionNotValid", 26 },
    { "DataEncodingSchemeIsNotSupported", 27 },
    { "DataStorageTypeIsNotSupported", 28 },
    { "DeltaPNGNotSupported", 29 },
    { "EncryptedWPGImageFileNotSupported", 30 },
    { "FractalCompressionNotSupported", 31 },
    { "ImageColumnOrRowSizeIsNotSupported", 32 },
    { "ImageDoesNotHaveAMatteChannel", 33 },
    { "ImageIsNotTiled", 34 },
    { "ImageTypeNotSupported", 35 },
    { "IncompatibleSizeOfDouble", 36 },
    { "IrregularChannelGeometryNotSupported", 37 },
    { "JNGCompressionNotSupported", 38 },
    { "JPEGCompressionNotSupported", 39 },
    { "JPEGEmbeddingFailed", 40 },
    { "LocationTypeIsNotSupported", 41 },
    { "MapStorageTypeIsNotSupported", 42 },
    { "MSBByteOrderNotSupported", 43 },
    { "MultidimensionalMatricesAreNotSupported", 44 },
    { "MultipleRecordListNotSupported", 45 },
    { "No8BIMDataIsAvailable", 46 },
    { "NoAPP1DataIsAvailable", 47 },
    { "NoBitmapOnClipboard", 48 },
    { "NoColorProfileAvailable", 49 },
    { "NoDataReturned", 50 },
    { "NoImageVectorGraphics", 51 },
    { "NoIPTCInfoWasFound", 52 },
    { "NoIPTCProfileAvailable", 53 },
    { "NotASupportedImageFile", 54 },
    { "NumberOfImagesIsNotSupported", 55 },
    { "OnlyContinuousTonePictureSupported", 56 },
    { "OnlyLevelZerofilesSupported", 57 },
    { "PNGCompressionNotSupported", 58 },
    { "PNGLibraryTooOld", 59 },
    { "RLECompressionNotSupported", 60 },
    { "UnableToCopyProfile", 61 },
    { "UnableToCreateADC", 62 },
    { "UnableToCreateBitmap", 63 },
    { "UnableToDecompressImage", 64 },
    { "UnableToInitializeFPXLibrary", 65 },
    { "UnableToOpenBlob", 66 },
    { "UnableToReadAspectRatio", 67 },
    { "UnableToReadCIELABImages", 68 },
    { "UnableToReadSummaryInfo", 69 },
    { "UnableToSetAffineMatrix", 70 },
    { "UnableToSetAspectRatio", 71 },
    { "UnableToSetColorTwist", 72 },
    { "UnableToSetContrast", 73 },
    { "UnableToSetFilteringValue", 74 },
    { "UnableToSetImageComments", 75 },
    { "UnableToSetImageTitle", 76 },
    { "UnableToSetJPEGLevel", 77 },
    { "UnableToSetRegionOfInterest", 78 },
    { "UnableToSetSummaryInfo", 79 },
    { "UnableToTranslateText", 80 },
    { "UnableToWriteMPEGParameters", 81 },
    { "UnableToZipCompressImage", 82 },
    { "UnsupportedCellTypeInTheMatrix", 83 },
    { "ZipCompressionNotSupported", 84 },
    { "Default", 85 },
    { "LosslessToLossyJPEGConversion", 86 },
    { "IncludeElementNestedTooDeeply", 87 },
    { "RegistryKeyLookupFailed", 88 },
    { "UnableToAccessConfigureFile", 89 },
    { "UnableToAccessFontFile", 90 },
    { "UnableToAccessLogFile", 91 },
    { "UnableToAccessModuleFile", 92 },
    { "Default", 93 },
    { "Default", 94 },
    { "AnErrorHasOccurredReadingFromFile", 95 },
    { "AnErrorHasOccurredWritingToFile", 96 },
    { "ColormapExceeds256Colors", 97 },
    { "CorruptImage", 98 },
    { "ImageFileDoesNotContainAnyImageData", 99 },
    { "ImageTypeNotSupported", 100 },
    { "ImproperImageHeader", 101 },
    { "InsufficientImageDataInFile", 102 },
    { "InvalidColormapIndex", 103 },
    { "InvalidFileFormatVersion", 104 },
    { "LengthAndFilesizeDoNotMatch", 105 },
    { "MissingImageChannel", 106 },
    { "NegativeOrZeroImageSize", 107 },
    { "NonOS2HeaderSizeError", 108 },
    { "NotABMPImageFile", 109 },
    { "NotACUTImageFile", 110 },
    { "NotADCMImageFile", 111 },
    { "NotADCXImageFile", 112 },
    { "NotADIBImageFile", 113 },
    { "NotADPXImageFile", 114 },
    { "NotAGIFImageFile", 115 },
    { "NotAJNGImageFile", 116 },
    { "NotAMATLABImageFile", 117 },
    { "NotAMNGImageFile", 118 },
    { "NotAMTVImageFile", 119 },
    { "NotAnARTImageFile", 120 },
    { "NotAnAVIImageFile", 121 },
    { "NotAnAVSImageFile", 122 },
    { "NotAnEMFImageFile", 123 },
    { "NotAnICOImageFile", 124 },
    { "NotAOTBImageFile", 125 },
    { "NotAPCDImageFile", 126 },
    { "NotAPCXImageFile", 127 },
    { "NotAPDBImageFile", 128 },
    { "NotAPICTImageFile", 129 },
    { "NotAPIXImageFile", 130 },
    { "NotAPNGImageFile", 131 },
    { "NotAPNMImageFile", 132 },
    { "NotAPSDImageFile", 133 },
    { "NotAPWPImageFile", 134 },
    { "NotARLEImageFile", 135 },
    { "NotASCTImageFile", 136 },
    { "NotASFWImageFile", 137 },
    { "NotASGIImageFile", 138 },
    { "NotASUNImageFile", 139 },
    { "NotATGAImageFile", 140 },
    { "NotATIMImageFile", 141 },
    { "NotATOPOLImageFile", 142 },
    { "NotAVIFFImageFile", 143 },
    { "NotAWBMPImageFile", 144 },
    { "NotAWPGImageFile", 145 },
    { "NotAXBMImageFile", 146 },
    { "NotAXCFImageFile", 147 },
    { "NotAXPMImageFile", 148 },
    { "NotEnoughTiles", 149 },
    { "StaticPlanesValueNotEqualToOne", 150 },
    { "TooMuchImageDataInFile", 151 },
    { "UnableToReadColormapFromDumpFile", 152 },
    { "UnableToReadColorProfile", 153 },
    { "UnableToReadExtensionBlock", 154 },
    { "UnableToReadGenericProfile", 155 },
    { "UnableToReadImageData", 156 },
    { "UnableToReadImageHeader", 157 },
    { "UnableToReadIPTCProfile", 158 },
    { "UnableToReadPixmapFromDumpFile", 159 },
    { "UnableToReadSubImageData", 160 },
    { "UnableToReadVIDImage", 161 },
    { "UnableToReadWindowNameFromDumpFile", 162 },
    { "UnableToRunlengthDecodeImage", 163 },
    { "UnableToUncompressImage", 164 },
    { "UnexpectedEndOfFile", 165 },
    { "UnexpectedSamplingFactor", 166 },
    { "UnknownPatternType", 167 },
    { "UnrecognizedBitsPerPixel", 168 },
    { "UnrecognizedImageCompression", 169 },
    { "UnrecognizedXWDHeader", 170 },
    { "UnableToPersistKey", 171 },
    { "CompressionNotValid", 172 },
    { "ImproperImageHeader", 173 },
    { "NegativeOrZeroImageSize", 174 },
    { "NonOS2HeaderSizeError", 175 },
    { "SkipToSyncByte", 176 },
    { "StaticPlanesValueNotEqualToOne", 177 },
    { "UnrecognizedBitsPerPixel", 178 },
    { "UnrecognizedImageCompression", 179 },
    { "UnrecognizedNumberOfColors", 180 },
    { "DelegateFailed", 181 },
    { "FailedToComputeOutputSize", 182 },
    { "FailedToRenderFile", 183 },
    { "FailedToScanFile", 184 },
    { "NoTagFound", 185 },
    { "PostscriptDelegateFailed", 186 },
    { "UnableToCreateImage", 187 },
    { "UnableToCreateImageComponent", 188 },
    { "UnableToDecodeImageFile", 189 },
    { "UnableToEncodeImageFile", 190 },
    { "UnableToInitializeFPXLibrary", 191 },
    { "UnableToInitializeWMFLibrary", 192 },
    { "UnableToManageJP2Stream", 193 },
    { "UnableToWriteSVGFormat", 194 },
    { "Default", 195 },
    { "Default", 196 },
    { "AlreadyPushingPatternDefinition", 197 },
    { "NonconformingDrawingPrimitiveDefinition", 198 },
    { "UnableToPrint", 199 },
    { "UnbalancedGraphicContextPushPop", 200 },
    { "Default", 201 },
    { "NotARelativeURL", 202 },
    { "NotCurrentlyPushingPatternDefinition", 203 },
    { "URLNotFound", 204 },
    { "UnableToCreateTemporaryFile", 205 },
    { "UnableToOpenFile", 206 },
    { "UnableToWriteFile", 207 },
    { "Default", 208 },
    { "Default", 209 },
    { "AngleIsDiscontinuous", 210 },
    { "ColorspaceColorProfileMismatch", 211 },
    { "ImageColorspaceDiffers", 212 },
    { "ImageOpacityDiffers", 213 },
    { "ImageSequenceIsRequired", 214 },
    { "ImageSizeDiffers", 215 },
    { "InvalidColormapIndex", 216 },
    { "LeftAndRightImageSizesDiffer", 217 },
    { "NoImagesWereFound", 218 },
    { "NoImagesWereLoaded", 219 },
    { "NoLocaleImageAttribute", 220 },
    { "TooManyClusters", 221 },
    { "UnableToAppendImage", 222 },
    { "UnableToAssignProfile", 223 },
    { "UnableToAverageImage", 224 },
    { "UnableToCoalesceImage", 225 },
    { "UnableToCompareImages", 226 },
    { "UnableToCreateImageMosaic", 227 },
    { "UnableToCreateStereoImage", 228 },
    { "UnableToDeconstructImageSequence", 229 },
    { "UnableToFlattenImage", 230 },
    { "UnableToResizeImage", 231 },
    { "UnableToSegmentImage", 232 },
    { "UnableToSetClipMask", 233 },
    { "UnableToShearImage", 234 },
    { "WidthOrHeightExceedsLimit", 235 },
    { "UnableToPersistKey", 236 },
    { "Default", 237 },
    { "DPSLibraryIsNotAvailable", 238 },
    { "FPXLibraryIsNotAvailable", 239 },
    { "FreeTypeLibraryIsNotAvailable", 240 },
    { "JPEGLibraryIsNotAvailable", 241 },
    { "LCMSLibraryIsNotAvailable", 242 },
    { "LZWEncodingNotEnabled", 243 },
    { "NoDecodeDelegateForThisImageFormat", 244 },
    { "NoEncodeDelegateForThisImageFormat", 245 },
    { "TIFFLibraryIsNotAvailable", 246 },
    { "XMLLibraryIsNotAvailable", 247 },
    { "XWindowLibraryIsNotAvailable", 248 },
    { "ZipLibraryIsNotAvailable", 249 },
    { "Default", 250 },
    { "Default", 251 },
    { "FailedToCloseModule", 252 },
    { "FailedToFindSymbol", 253 },
    { "UnableToLoadModule", 254 },
    { "UnableToRegisterImageFormat", 255 },
    { "UnrecognizedModule", 256 },
    { "UnableToInitializeModuleLoader", 257 },
    { "Default", 258 },
    { "BevelWidthIsNegative", 259 },
    { "ColorSeparatedImageRequired", 260 },
    { "FrameIsLessThanImageSize", 261 },
    { "GeometryDimensionsAreZero", 262 },
    { "GeometryDoesNotContainImage", 263 },
    { "ImagesAreNotTheSameSize", 264 },
    { "ImageSizeMustExceedBevelWidth", 265 },
    { "ImageSmallerThanKernelWidth", 266 },
    { "ImageSmallerThanRadius", 267 },
    { "ImageWidthsOrHeightsDiffer", 268 },
    { "InputImagesAlreadySpecified", 269 },
    { "KernelRadiusIsTooSmall", 270 },
    { "KernelWidthMustBeAnOddNumber", 271 },
    { "MissingAffineMatrix", 272 },
    { "MissingAmount", 273 },
    { "MissingAmplitude", 274 },
    { "MissingAnImageFilename", 275 },
    { "MissingArgument", 276 },
    { "MissingAzimuth", 277 },
    { "MissingBackgroundColor", 278 },
    { "MissingBevelWidth", 279 },
    { "MissingBorderColor", 280 },
    { "MissingClippingPathName", 281 },
    { "MissingCoefficients", 282 },
    { "MissingColorizeValue", 283 },
    { "MissingColors", 284 },
    { "MissingComment", 285 },
    { "MissingCompressionQuality", 286 },
    { "MissingCycleAmount", 287 },
    { "MissingDelay", 288 },
    { "MissingDisposeMethod", 289 },
    { "MissingDissolveValue", 290 },
    { "MissingDistance", 291 },
    { "MissingDrawingPrimitive", 292 },
    { "MissingEnodingType", 293 },
    { "MissingEventMask", 294 },
    { "MissingFilename", 295 },
    { "MissingFillColor", 296 },
    { "MissingFontName", 297 },
    { "MissingFormatSpecifier", 298 },
    { "MissingGeometry", 299 },
    { "MissingImageDepth", 300 },
    { "MissingImageFrames", 301 },
    { "MissingImageGamma", 302 },
    { "MissingImageIterations", 303 },
    { "MissingImageLabel", 304 },
    { "MissingImageProfile", 305 },
    { "MissingImageRotation", 306 },
    { "MissingImageScene", 307 },
    { "MissingImageType", 308 },
    { "MissingImplodeAmount", 309 },
    { "MissingListName", 310 },
    { "MissingLogFormat", 311 },
    { "MissingMatrix", 312 },
    { "MissingMatteColor", 313 },
    { "MissingOffset", 314 },
    { "MissingOpaqueColor", 315 },
    { "MissingOrder", 316 },
    { "MissingPageGeometry", 317 },
    { "MissingPaintRadius", 318 },
    { "MissingPassword", 319 },
    { "MissingPoint", 320 },
    { "MissingPointsize", 321 },
    { "MissingResourceLimit", 322 },
    { "MissingResourceType", 323 },
    { "MissingSeedValue", 324 },
    { "MissingServerName", 325 },
    { "MissingSpreadAmount", 326 },
    { "MissingThreshold", 327 },
    { "MissingTile", 328 },
    { "MissingTitle", 329 },
    { "MissingTransparentColor", 330 },
    { "MissingTreeDepth", 331 },
    { "MissingType", 332 },
    { "MissingViewTransform", 333 },
    { "MissingVirtualPixelMethod", 334 },
    { "MissingWidth", 335 },
    { "MustSpecifyAnImageName", 336 },
    { "MustSpecifyImageSize", 337 },
    { "NoBlobDefined", 338 },
    { "NoImagesDefined", 339 },
    { "NonzeroWidthAndHeightRequired", 340 },
    { "NoProfileNameWasGiven", 341 },
    { "NullBlobArgument", 342 },
    { "ReferenceImageRequired", 343 },
    { "ReferenceIsNotMyType", 344 },
    { "SteganoImageRequired", 345 },
    { "StereoImageRequired", 346 },
    { "SubimageSpecificationReturnsNoImages", 347 },
    { "UnableToAddOrRemoveProfile", 348 },
    { "UnableToAllocateICCProfile", 349 },
    { "UnableToAverageImageSequence", 350 },
    { "UnableToBlurImage", 351 },
    { "UnableToChopImage", 352 },
    { "UnableToConstituteImage", 353 },
    { "UnableToConvolveImage", 354 },
    { "UnableToEdgeImage", 355 },
    { "UnableToEqualizeImage", 356 },
    { "UnableToFilterImage", 357 },
    { "UnableToFormatImageMetadata", 358 },
    { "UnableToFrameImage", 359 },
    { "UnableToOilPaintImage", 360 },
    { "UnableToPaintImage", 361 },
    { "UnableToRaiseImage", 362 },
    { "UnableToSharpenImage", 363 },
    { "UnableToThresholdImage", 364 },
    { "UnableToWaveImage", 365 },
    { "UnrecognizedAttribute", 366 },
    { "UnrecognizedChannelType", 367 },
    { "UnrecognizedColor", 368 },
    { "UnrecognizedColormapType", 369 },
    { "UnrecognizedColorspace", 370 },
    { "UnrecognizedComposeOperator", 371 },
    { "UnrecognizedDisposeMethod", 372 },
    { "UnrecognizedElement", 373 },
    { "UnrecognizedEndianType", 374 },
    { "UnrecognizedGravityType", 375 },
    { "UnrecognizedImageCompression", 376 },
    { "UnrecognizedImageFilter", 377 },
    { "UnrecognizedImageFormat", 378 },
    { "UnrecognizedImageMode", 379 },
    { "UnrecognizedImageType", 380 },
    { "UnrecognizedIntentType", 381 },
    { "UnrecognizedInterlaceType", 382 },
    { "UnrecognizedListType", 383 },
    { "UnrecognizedModeType", 384 },
    { "UnrecognizedNoiseType", 385 },
    { "UnrecognizedOption", 386 },
    { "UnrecognizedPerlMagickMethod", 387 },
    { "UnrecognizedPixelMap", 388 },
    { "UnrecognizedPreviewType", 389 },
    { "UnrecognizedResourceType", 390 },
    { "UnrecognizedType", 391 },
    { "UnrecognizedVirtualPixelMethod", 392 },
    { "UsageError", 393 },
    { "DeprecatedOption", 394 },
    { "InvalidColorspaceType", 395 },
    { "InvalidEndianType", 396 },
    { "InvalidImageType", 397 },
    { "InvalidInterlaceType", 398 },
    { "MissingAnImageFilename", 399 },
    { "MissingBevelWidth", 400 },
    { "MissingBorderColor", 401 },
    { "MissingColor", 402 },
    { "MissingColors", 403 },
    { "MissingCommand", 404 },
    { "MissingComment", 405 },
    { "MissingCompressionQuality", 406 },
    { "MissingDegrees", 407 },
    { "MissingDepth", 408 },
    { "MissingDisposeMethod", 409 },
    { "MissingEncodingType", 410 },
    { "MissingEventMask", 411 },
    { "MissingFactor", 412 },
    { "MissingFilename", 413 },
    { "MissingFontName", 414 },
    { "MissingForeground", 415 },
    { "MissingGeometry", 416 },
    { "MissingIDNameOrRoot", 417 },
    { "MissingImageDepth", 418 },
    { "MissingLabelName", 419 },
    { "MissingLevel", 420 },
    { "MissingLogFormat", 421 },
    { "MissingMapType", 422 },
    { "MissingMethod", 423 },
    { "MissingName", 424 },
    { "MissingPageGeometry", 425 },
    { "MissingPassword", 426 },
    { "MissingQuality", 427 },
    { "MissingResourceLimit", 428 },
    { "MissingResourceType", 429 },
    { "MissingSceneNumber", 430 },
    { "MissingSeconds", 431 },
    { "MissingServerName", 432 },
    { "MissingSize", 433 },
    { "MissingSnaps", 434 },
    { "MissingThreshold", 435 },
    { "MissingTitle", 436 },
    { "MissingType", 437 },
    { "MissingValue", 438 },
    { "MissingVirtualPixelMethod", 439 },
    { "MissingVisualClass", 440 },
    { "MissingWidth", 441 },
    { "NoImagesWereLoaded", 442 },
    { "OptionLengthExceedsLimit", 443 },
    { "UnableToOpenXServer", 444 },
    { "UnableToPersistKey", 445 },
    { "UnrecognizedColormapType", 446 },
    { "UnrecognizedColorspaceType", 447 },
    { "UnrecognizedDisposeMethod", 448 },
    { "UnrecognizedEndianType", 449 },
    { "UnrecognizedFilterType", 450 },
    { "UnrecognizedImageCompressionType", 451 },
    { "UnrecognizedImageType", 452 },
    { "UnrecognizedInterlaceType", 453 },
    { "UnrecognizedOption", 454 },
    { "UnrecognizedResourceType", 455 },
    { "UnrecognizedVirtualPixelMethod", 456 },
    { "UnrecognizedColor", 457 },
    { "ImageExpected", 458 },
    { "ImageInfoExpected", 459 },
    { "StructureSizeMismatch", 460 },
    { "UnableToGetRegistryID", 461 },
    { "UnableToLocateImage", 462 },
    { "UnableToSetRegistry", 463 },
    { "Default", 464 },
    { "Default", 465 },
    { "CacheResourcesExhausted", 466 },
    { "MemoryAllocationFailed", 467 },
    { "NoPixelsDefinedInCache", 468 },
    { "PixelCacheAllocationFailed", 469 },
    { "UnableToAddColorProfile", 470 },
    { "UnableToAddGenericProfile", 471 },
    { "UnableToAddIPTCProfile", 472 },
    { "UnableToAllocateCoefficients", 473 },
    { "UnableToAllocateColormap", 474 },
    { "UnableToAllocateString", 475 },
    { "UnableToAnnotateImage", 476 },
    { "UnableToAverageImageSequence", 477 },
    { "UnableToCloneImage", 478 },
    { "UnableToComputeImageSignature", 479 },
    { "UnableToConstituteImage", 480 },
    { "UnableToConvertFont", 481 },
    { "UnableToConvertStringToTokens", 482 },
    { "UnableToCreateColormap", 483 },
    { "UnableToCreateColorTransform", 484 },
    { "UnableToCreateCommandWidget", 485 },
    { "UnableToCreateImageGroup", 486 },
    { "UnableToCreateImageMontage", 487 },
    { "UnableToCreateXWindow", 488 },
    { "UnableToCropImage", 489 },
    { "UnableToDespeckleImage", 490 },
    { "UnableToDetermineImageClass", 491 },
    { "UnableToDetermineTheNumberOfImageColors", 492 },
    { "UnableToDitherImage", 493 },
    { "UnableToDrawOnImage", 494 },
    { "UnableToEdgeImage", 495 },
    { "UnableToEmbossImage", 496 },
    { "UnableToEnhanceImage", 497 },
    { "UnableToFloodfillImage", 498 },
    { "UnableToGammaCorrectImage", 499 },
    { "UnableToGetBestIconSize", 500 },
    { "UnableToGetFromRegistry", 501 },
    { "UnableToGetPackageInfo", 502 },
    { "UnableToLevelImage", 503 },
    { "UnableToMagnifyImage", 504 },
    { "UnableToManageColor", 505 },
    { "UnableToMapImage", 506 },
    { "UnableToMapImageSequence", 507 },
    { "UnableToMedianFilterImage", 508 },
    { "UnableToMotionBlurImage", 509 },
    { "UnableToNoiseFilterImage", 510 },
    { "UnableToNormalizeImage", 511 },
    { "UnableToOpenColorProfile", 512 },
    { "UnableToQuantizeImage", 513 },
    { "UnableToQuantizeImageSequence", 514 },
    { "UnableToReadTextChunk", 515 },
    { "UnableToReadXImage", 516 },
    { "UnableToReadXServerColormap", 517 },
    { "UnableToResizeImage", 518 },
    { "UnableToRotateImage", 519 },
    { "UnableToSampleImage", 520 },
    { "UnableToScaleImage", 521 },
    { "UnableToSelectImage", 522 },
    { "UnableToSharpenImage", 523 },
    { "UnableToShaveImage", 524 },
    { "UnableToShearImage", 525 },
    { "UnableToSortImageColormap", 526 },
    { "UnableToThresholdImage", 527 },
    { "UnableToTransformColorspace", 528 },
    { "MemoryAllocationFailed", 529 },
    { "UnableToAllocateAscii85Info", 530 },
    { "UnableToAllocateCacheInfo", 531 },
    { "UnableToAllocateCacheView", 532 },
    { "UnableToAllocateColorInfo", 533 },
    { "UnableToAllocateDashPattern", 534 },
    { "UnableToAllocateDelegateInfo", 535 },
    { "UnableToAllocateDerivatives", 536 },
    { "UnableToAllocateDrawContext", 537 },
    { "UnableToAllocateDrawInfo", 538 },
    { "UnableToAllocateDrawingWand", 539 },
    { "UnableToAllocateGammaMap", 540 },
    { "UnableToAllocateImage", 541 },
    { "UnableToAllocateImagePixels", 542 },
    { "UnableToAllocateLogInfo", 543 },
    { "UnableToAllocateMagicInfo", 544 },
    { "UnableToAllocateMagickInfo", 545 },
    { "UnableToAllocateModuleInfo", 546 },
    { "UnableToAllocateMontageInfo", 547 },
    { "UnableToAllocateQuantizeInfo", 548 },
    { "UnableToAllocateRegistryInfo", 549 },
    { "UnableToAllocateSemaphoreInfo", 550 },
    { "UnableToAllocateString", 551 },
    { "UnableToAllocateTypeInfo", 552 },
    { "UnableToAnimateImageSequence", 553 },
    { "UnableToCloneBlobInfo", 554 },
    { "UnableToCloneCacheInfo", 555 },
    { "UnableToCloneImage", 556 },
    { "UnableToCloneImageInfo", 557 },
    { "UnableToConcatenateString", 558 },
    { "UnableToConvertText", 559 },
    { "UnableToCreateColormap", 560 },
    { "UnableToDisplayImage", 561 },
    { "UnableToEscapeString", 562 },
    { "UnableToInterpretMSLImage", 563 },
    { "MemoryAllocationFailed", 564 },
    { "ImageDoesNotContainTheStreamGeometry", 565 },
    { "NoStreamHandlerIsDefined", 566 },
    { "PixelCacheIsNotOpen", 567 },
    { "UnableToAcquirePixelStream", 568 },
    { "UnableToSetPixelStream", 569 },
    { "UnableToSyncPixelStream", 570 },
    { "Default", 571 },
    { "Default", 572 },
    { "FontSubstitutionRequired", 573 },
    { "UnableToGetTypeMetrics", 574 },
    { "UnableToInitializeFreetypeLibrary", 575 },
    { "UnableToReadFont", 576 },
    { "UnrecognizedFontEncoding", 577 },
    { "Default", 578 },
    { "Default", 579 },
    { "ColorIsNotKnownToServer", 580 },
    { "NoWindowWithSpecifiedIDExists", 581 },
    { "StandardColormapIsNotInitialized", 582 },
    { "UnableToConnectToRemoteDisplay", 583 },
    { "UnableToCreateBitmap", 584 },
    { "UnableToCreateColormap", 585 },
    { "UnableToCreatePixmap", 586 },
    { "UnableToCreateProperty", 587 },
    { "UnableToCreateStandardColormap", 588 },
    { "UnableToDisplayImageInfo", 589 },
    { "UnableToGetProperty", 590 },
    { "UnableToGetStandardColormap", 591 },
    { "UnableToGetVisual", 592 },
    { "UnableToGrabMouse", 593 },
    { "UnableToLoadFont", 594 },
    { "UnableToMatchVisualToStandardColormap", 595 },
    { "UnableToOpenXServer", 596 },
    { "UnableToReadXAttributes", 597 },
    { "UnableToReadXWindowImage", 598 },
    { "UnrecognizedColormapType", 599 },
    { "UnrecognizedGravityType", 600 },
    { "UnrecognizedVisualSpecifier", 601 },
    { "UnableToAllocateXHints", 602 },
    { "UnableToCreateCursor", 603 },
    { "UnableToCreateGraphicContext", 604 },
    { "UnableToCreateStandardColormap", 605 },
    { "UnableToCreateTextProperty", 606 },
    { "UnableToCreateXImage", 607 },
    { "UnableToCreateXPixmap", 608 },
    { "UnableToCreateXWindow", 609 },
    { "UnableToDisplayImage", 610 },
    { "UnableToDitherImage", 611 },
    { "UnableToGetPixelInfo", 612 },
    { "UnableToGetVisual", 613 },
    { "UnableToLoadFont", 614 },
    { "UnableToMakeXWindow", 615 },
    { "UnableToOpenXServer", 616 },
    { "UnableToViewFonts", 617 },
    { "UnableToGetVisual", 618 },
    { "UsingDefaultVisual", 619 },
    { 0, 0 }
  };
#endif

#if defined(_INCLUDE_MESSAGE_TABLE_)
static const char *message_dat[] =
  {
    "%1",
    "Unable to create blob",
    "Unable to open file",
    "Unable to read file",
    "Unable to read to offset",
    "Unable to seek to offset",
    "Unable to write blob",
    "Unrecognized image format",
    "default error",
    "default warning",
    "Inconsistent persistent cache depth",
    "Pixel cache is not open",
    "Unable to clone cache",
    "Unable to extend cache",
    "Unable to get cache nexus",
    "Unable to get pixels from cache",
    "Unable to open cache",
    "Unable to persist pixel cache",
    "Unable to read pixel cache",
    "Unable to sync cache",
    "disk allocation failed",
    "Unable to extend pixel cache",
    "default warning",
    "Colormap type not supported",
    "Colorspace model is not supported",
    "Color type not supported",
    "Compression not valid",
    "Data encoding scheme is not supported",
    "Data storage type is not supported",
    "Delta-PNG is not supported",
    "Encrypted WPG image file not supported",
    "Fractal compression not supported",
    "Image column or row size is not supported",
    "Image does not have a matte channel",
    "Image is not tiles",
    "Image type not supported",
    "Incompatible size of double",
    "Irregular channel geometry not supported",
    "JNG compression is not supported",
    "JPEG compression is not supported",
    "JPEG embedding failed",
    "Location type is not supported",
    "Map storage type is not supported",
    "MSB order not supported bitmap",
    "Multi-dimensional matrices are not supported",
    "Multiple record list not supported",
    "No 8BIM data is available",
    "No APP1 data is available",
    "No bitmap on clipboard",
    "No color profile available",
    "No data returned",
    "No image vector graphics",
    "No IPTC info was found",
    "No IPTC profile available",
    "Not a supported image file",
    "Number of images is not supported",
    "Only continuous tone picture supported",
    "Only level zero files Supported",
    "PNG compression is not supported",
    "PNG library is too old",
    "RLE compression not supported",
    "Unable to copy profile",
    "Unable to create a DC",
    "Unable to create bitmap",
    "Unable to decompress image",
    "Unable to Initialize FPX library",
    "Unable to open blob",
    "Unable to read aspect ratio",
    "Unable to read CIELAB images",
    "Unable to read summary info",
    "Unable to set affine matrix",
    "Unable to set aspect ratio",
    "Unable to set color twist",
    "Unable to set contrast",
    "Unable to set filtering value",
    "Unable to set image comment",
    "Unable to set image title",
    "Unable to set JPEG level",
    "Unable to set region of interest",
    "Unable to set summary info",
    "Unable to translate text",
    "Unable to write MPEG parameters",
    "Unable to zip-compress image",
    "Unsupported cell type in the matrix",
    "ZIP compression is not supported",
    "default error",
    "Lossless to lossy JPEG conversion",
    "include element nested too deeply",
    "Registry key lookup failed. Package is not properly installed on this machine.",
    "Unable to access configuration file",
    "Unable to access font file",
    "Unable to access log configuration file",
    "Unable to access module file",
    "default error",
    "default warning",
    "An error has occurred reading from file",
    "An error has occurred writing to file",
    "Colormap exceeded 256 colors",
    "Corrupt image",
    "Image file or does not contain any image data",
    "Image type not supported",
    "Improper image header",
    "Insufficient image data in file",
    "Invalid colormap index",
    "invalid file format version",
    "Length and filesize do not match",
    "Missing a required image channel",
    "Negative or zero image size",
    "Non OS2 BMP header size less than 40",
    "Not a BMP image file",
    "Not a CUT image file",
    "Not a DCM image file",
    "Not a DCX image file",
    "Not a DIB image file",
    "Not a DPX image file",
    "Not a GIF image file",
    "Not a JNG image file",
    "Not a MATLAB image file",
    "Not a MNG image file",
    "Not a MTV image file",
    "Not an ART image file",
    "Not an AVI image file",
    "Not an AVS image file",
    "Not an EMF image file",
    "Not a ICO image file",
    "Not a OTB image file",
    "Not a PCD image file",
    "Not a PCX image file",
    "Not a PDB image file",
    "Not a PICT image file",
    "Not a PIX image file",
    "Not a PNG image file",
    "Not a PNM image file",
    "Not a PSD image file",
    "Not a PSP image file",
    "Not a RLE image file",
    "Not a SCT image file",
    "Not a SFW image file",
    "Not a SGI image file",
    "Not a SUN image file",
    "Not a TGA image file",
    "Not a TIM image file",
    "Not a TOPAL image file",
    "Not a VIFF image file",
    "Not a WBMP image file",
    "Not a WPG image file",
    "Not a XBM image file",
    "Not a XCF image file",
    "Not a XPM image file",
    "Not enough tiles found in level",
    "Static planes value not equal to 1",
    "Too much image data in file",
    "Unable to read colormap from dump file",
    "Unable to read color profile",
    "Unable to read extension block",
    "Unable to read generic profile",
    "Unable to read image data",
    "Unable to read image header",
    "Unable to read IPTC profile",
    "Unable to read pixmap from dump file",
    "Unable to read sub image data",
    "Unable to read VID image",
    "Unable to read window name from dump file",
    "Unable to runlength decode image",
    "Unable to uncompress image",
    "Unexpected end-of-file",
    "Unexpected sampling factor",
    "Unknown pattern type",
    "Unrecognized bits per pixel",
    "Unrecognized compression",
    "Unrecognized XWD header",
    "Unable to persist key",
    "Compression not valid",
    "Improper image header",
    "Negative or zero image size",
    "Non OS2 header size error",
    "Corrupt PCD image, skipping to sync byte",
    "Static planes value not equal to one",
    "Unrecognized bits per pixel",
    "Unrecognized image compression",
    "Unrecognized number of colors",
    "Delegate failed",
    "Failed to compute output size",
    "Failed to render file",
    "Failed to scan file",
    "No tag found",
    "Postscript delegate failed",
    "Unable to create image",
    "Unable to create image component",
    "Unable to decode image file",
    "Unable to encode image file",
    "Unable to initialize FPX library",
    "Unable to initialize WMF library",
    "Unable to manage JP2 stream",
    "Unable to write SVG format",
    "default error",
    "default warning",
    "Already pushing pattern definition",
    "Non-conforming drawing primitive definition",
    "Unable to print",
    "unbalanced graphic context push-pop",
    "default error",
    "Not a relative URL",
    "Not currently pushing pattern definition",
    "URL not found",
    "Unable to create temporary file",
    "Unable to open file",
    "Unable to write file",
    "default error",
    "default warning",
    "angle is discontinuous",
    "Colorspace color profile mismatch",
    "image colorspace differs",
    "image opacity differs",
    "Image sequence is required",
    "image size differs",
    "Invalid colormap index",
    "left and right image sizes differ",
    "no images were found",
    "no images were loaded",
    "no [LOCALE] image attribute",
    "too many cluster",
    "unable to append image",
    "Unable to assign profile",
    "unable to average image",
    "unable to coalesce image",
    "unable to compare images",
    "unable to create image mosaic",
    "unable to create stereo image",
    "unable to deconstruct image sequence",
    "unable to flatten image",
    "unable to resize image",
    "unable to segment image",
    "Unable to set clip mask",
    "unable to shear image",
    "width or height exceeds limit",
    "Unable to persist key",
    "default warning",
    "DPS library is not available",
    "FPX library is not available",
    "FreeType library is not available",
    "JPEG compression library is not available",
    "LCMS encoding not enabled",
    "LZW encoding not enabled",
    "No decode delegate for this image format",
    "No encode delegate for this image format",
    "TIFF library is not available",
    "XML library is not available",
    "X Window library is not available",
    "ZLIB compression library is not available",
    "default error",
    "default warning",
    "Failed to close module",
    "Failed to find symbol",
    "Unable to load module",
    "Unable to register image format",
    "Unrecognized module",
    "Unable to initialize module loader",
    "default warning",
    "bevel width is negative",
    "color separated image required",
    "frame is less than image size",
    "geometry dimensions are zero",
    "geometry does not contain image",
    "images are not the same size",
    "size must exceed bevel width",
    "image smaller than kernel width",
    "image smaller than radius",
    "image widths or heights differ",
    "input images already specified",
    "kernel radius is too small",
    "kernel width must be an odd number",
    "Missing affine matrix",
    "Missing amount",
    "Missing amplitude",
    "Missing an image filename",
    "Missing argument",
    "Missing azimuth",
    "Missing background color",
    "Missing bevel width",
    "Missing border color",
    "Missing clipping path name",
    "Missing coefficients",
    "Missing colorize value",
    "Missing colors",
    "Missing comment",
    "Missing compression quality",
    "Missing an cycle amount",
    "Missing delay",
    "Missing dispose method",
    "Missing dissolve value",
    "Missing distance",
    "Missing drawing primitive",
    "Missing encoding type",
    "Missing event mask",
    "Missing filename",
    "Missing fill color",
    "Missing font name",
    "Missing format specifier",
    "Missing geometry",
    "Missing image depth",
    "Missing image frames",
    "Missing image gamma",
    "Missing image iterations",
    "Missing image label",
    "Missing image profile",
    "Missing image rotation",
    "Missing image scene",
    "Missing image type",
    "Missing an implode amount",
    "Missing list name",
    "Missing log format",
    "Missing matrix",
    "Missing matte color",
    "Missing offset",
    "Missing opaque color",
    "Missing order",
    "Missing page geometry",
    "Missing paint radius",
    "Missing password",
    "Missing point",
    "Missing pointsize",
    "Missing resource limit",
    "Missing resource type",
    "Missing seed value",
    "Missing server name",
    "Missing an spread amount",
    "Missing threshold",
    "Missing tile",
    "Missing title",
    "Missing transparent color",
    "Missing tree depth",
    "Missing type",
    "Missing view transform",
    "Missing virtual pixel method",
    "Missing width",
    "Must specify a image name",
    "Must specify image size",
    "No Binary Large OBjects defined",
    "No images defined",
    "Non-zero width and height required",
    "No profile name was given",
    "Null blob argument",
    "Reference image required",
    "Reference is not my type",
    "Stegano image required",
    "Stereo image required",
    "Subimage specification returns no images",
    "Unable to add or remove profile",
    "unable to allocate ICC profile",
    "unable to average image sequence",
    "unable to blur image",
    "unable to chop image",
    "Unable to constitute image",
    "Unable to convolve image",
    "Unable to edge image",
    "Unable to equalize image",
    "Unable to filter image",
    "unable to format image meta data",
    "Unable to frame image",
    "unable to oil paint image",
    "Unable to paint image",
    "Unable to raise image",
    "Unable to sharpen image",
    "Unable to threshold image",
    "Unable to wave image",
    "Unrecognized attribute",
    "Unrecognized channel type",
    "Unrecognized color",
    "Unrecognized colormap type",
    "Unrecognized image colorspace",
    "Unrecognized compose operator",
    "Unrecognized dispose method",
    "Unrecognized element",
    "Unrecognized endian type",
    "Unrecognized gravity type",
    "Unrecognized image compression",
    "Unrecognized image filter",
    "Unrecognized image format",
    "Unrecognized image mode",
    "Unrecognized image type",
    "Unrecognized intent type",
    "Unrecognized interlace type",
    "Unrecognized list type",
    "Unrecognized mode type",
    "Unrecognized noise type",
    "Unrecognized option",
    "Unrecognized PerlMagick method",
    "Unrecognized pixel map",
    "Unrecognized preview type",
    "Unrecognized resource type",
    "Unrecognized type",
    "Unrecognized virtual pixel method",
    "Improper arguments supplied, please see manual",
    "Deprecated option",
    "Invalid colorspace type",
    "Invalid endian type",
    "Invalid image type",
    "Invalid interlace type",
    "Missing an image filename",
    "Missing bevel width",
    "Missing border color",
    "Missing color",
    "Missing colors",
    "Missing command",
    "Missing comment",
    "Missing compression quality",
    "Missing degrees",
    "Missing depth",
    "Missing dispose method",
    "Missing encoding type",
    "Missing event mask",
    "Missing factor",
    "Missing filename",
    "Missing font name",
    "Missing foreground",
    "Missing geometry",
    "Missing id name or 'root",
    "Missing image depth",
    "Missing label name",
    "Missing level",
    "Missing log format",
    "Missing map type",
    "Missing method",
    "Missing name",
    "Missing page geometry",
    "Missing password",
    "Missing quality",
    "Missing resource limit",
    "Missing resource limit",
    "Missing scene number",
    "Missing seconds",
    "Missing server name",
    "Missing size",
    "Missing snaps",
    "Missing threshold",
    "Missing title",
    "Missing type",
    "Missing value",
    "Missing virtual pixel method",
    "Missing visual class",
    "Missing width",
    "No images were loaded",
    "Option length exceeds limit",
    "Unable to open XServer",
    "Unable to persist key",
    "Unrecognized colormap type",
    "Unrecognized colorspace type",
    "unrecognized dispose method",
    "Unrecognized endian type",
    "Unrecognized filter type",
    "unrecognized compression type",
    "Unrecognized image type",
    "Unrecognized interlace type",
    "Unrecognized option",
    "Unrecognized resource type",
    "Unrecognized virtual pixel method",
    "Unrecognized color",
    "image expected",
    "image info expected",
    "structure size mismatch",
    "Unable to get registry ID",
    "Unable to locate image",
    "Unable to set registry",
    "default error",
    "default warning",
    "Cache resources exhausted",
    "Memory allocation failed",
    "No pixels defined in cache",
    "Pixel cache allocation failed",
    "unable to add ICC Color profile",
    "unable to add generic profile",
    "unable to add IPTC profile",
    "unable to allocate coefficients",
    "Unable to allocate colormap",
    "unable to allocate string",
    "Unable to annotate image",
    "unable to average image sequence",
    "unable to clone image",
    "unable to compute image signature",
    "unable to constitute image",
    "unable to convert font",
    "unable to convert strings to tokens",
    "Unable to create colormap",
    "unable to create color transform",
    "unable to create command widget",
    "unable to create image group",
    "Unable to create image montage",
    "unable to create X window",
    "unable to crop image",
    "unable to despeckle image",
    "unable to determine image class",
    "unable to determine the number of image colors",
    "unable to dither image",
    "unable to draw on image",
    "unable to edge image",
    "unable to emboss image",
    "unable to enhance image",
    "unable to floodfill image",
    "unable to gamma correct image",
    "unable to get best icon size",
    "unable to get from registry",
    "Unable to get package info",
    "unable to level image",
    "unable to magnify image",
    "Unable to manage color",
    "Unable to map image",
    "Unable to map image sequence",
    "unable to median filter image",
    "unable to motion blur image",
    "unable to noise filter image",
    "unable to normalize image",
    "unable to open color profile",
    "unable to quantize image",
    "unable to quantize image sequence",
    "unable to read text chunk",
    "unable to read X image",
    "unable to read X server colormap",
    "unable to resize image",
    "unable to rotate image",
    "unable to sample image",
    "unable to scale image",
    "unable to select image",
    "unable to sharpen image",
    "unable to shave image",
    "unable to shear image",
    "unable to sort image colormap",
    "unable to threshold image",
    "unable to transform colorspace",
    "Memory allocation failed",
    "unable to allocate ascii85 info",
    "unable to allocate cache info",
    "unable to allocate cache view",
    "unable to allocate color info",
    "unable to allocate dash pattern",
    "unable to allocate delegate info",
    "unable to allocate derivates",
    "unable to allocate draw context",
    "unable to allocate draw info",
    "unable to allocate drawing wand",
    "unable to allocate gamma map",
    "unable to allocate image",
    "unable to allocate image pixels",
    "unable to allocate log info",
    "unable to allocate magic info",
    "unable to allocate magick info",
    "unable to allocate module info",
    "unable to allocate montage info",
    "unable to allocate quantize info",
    "unable to allocate registry info",
    "unable to allocate semaphore info",
    "unable to allocate string",
    "unable to allocate type info",
    "unable to animate image sequence",
    "unable to clone blob info",
    "unable to clone cache info",
    "unable to clone image",
    "unable to clone image info",
    "unable to concatenate string",
    "unable to convert text",
    "unable to create colormap",
    "unable to display image",
    "unable to escape string",
    "unable to interpret MSL image",
    "Memory allocation failed",
    "image does not contain the stream geometry",
    "no stream handler is defined",
    "Pixel cache is not open",
    "Unable to acquire pixel stream",
    "Unable to set pixel stream",
    "Unable to sync pixel stream",
    "default error",
    "default warning",
    "Font substitution required",
    "Unable to get type metrics",
    "Unable to initialize freetype library",
    "Unable to read font",
    "Unrecognized font encoding",
    "default error",
    "default warning",
    "Color is not known to server",
    "No window with specified ID exists",
    "Standard Colormap is not initialized",
    "Unable to connect to remote display",
    "Unable to create bitmap",
    "Unable to create colormap",
    "Unable to create pixmap",
    "Unable to create property",
    "Unable to create standard colormap",
    "Unable to display image info",
    "Unable to get property",
    "Unable to get Standard Colormap",
    "Unable to get visual",
    "Unable to grab mouse",
    "Unable to load font",
    "Unable to match visual to Standard Colormap",
    "Unable to open X server",
    "Unable to read X attributes",
    "Unable to read X window image",
    "Unrecognized colormap type",
    "Unrecognized gravity type",
    "Unrecognized visual specifier",
    "Unable to allocate X hints",
    "Unable to create X cursor",
    "Unable to create graphic context",
    "unable to create standard colormap",
    "Unable to create text property",
    "Unable to create X image",
    "Unable to create X pixmap",
    "Unable to create X window",
    "unable to display image",
    "unable to dither image",
    "Unable to get pixel info",
    "Unable to get visual",
    "Unable to load font",
    "Unable to make X window",
    "Unable to open X server",
    "Unable to view fonts",
    "Unable to get visual",
    "UsingDefaultVisual",
    0
  };
#endif
