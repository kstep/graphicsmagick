#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 483

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
#define MGK_CoderErrorNumberOfImagesIsNotSupported 54
#define MGK_CoderErrorOnlyContinuousTonePictureSupported 55
#define MGK_CoderErrorOnlyLevelZerofilesSupported 56
#define MGK_CoderErrorPNGCompressionNotSupported 57
#define MGK_CoderErrorPNGLibraryTooOld 58
#define MGK_CoderErrorRLECompressionNotSupported 59
#define MGK_CoderErrorUnableToCopyProfile 60
#define MGK_CoderErrorUnableToCreateADC 61
#define MGK_CoderErrorUnableToCreateBitmap 62
#define MGK_CoderErrorUnableToDecompressImage 63
#define MGK_CoderErrorUnableToInitializeFPXLibrary 64
#define MGK_CoderErrorUnableToOpenBlob 65
#define MGK_CoderErrorUnableToReadAspectRatio 66
#define MGK_CoderErrorUnableToReadCIELABImages 67
#define MGK_CoderErrorUnableToReadSummaryInfo 68
#define MGK_CoderErrorUnableToSetAffineMatrix 69
#define MGK_CoderErrorUnableToSetAspectRatio 70
#define MGK_CoderErrorUnableToSetColorTwist 71
#define MGK_CoderErrorUnableToSetContrast 72
#define MGK_CoderErrorUnableToSetFilteringValue 73
#define MGK_CoderErrorUnableToSetImageComments 74
#define MGK_CoderErrorUnableToSetImageTitle 75
#define MGK_CoderErrorUnableToSetJPEGLevel 76
#define MGK_CoderErrorUnableToSetRegionOfInterest 77
#define MGK_CoderErrorUnableToSetSummaryInfo 78
#define MGK_CoderErrorUnableToTranslateText 79
#define MGK_CoderErrorUnableToWriteMPEGParameters 80
#define MGK_CoderErrorUnableToZipCompressImage 81
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 82
#define MGK_CoderErrorZipCompressionNotSupported 83
#define MGK_CoderFatalErrorDefault 84
#define MGK_CoderWarningLosslessToLossyJPEGConversion 85
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 86
#define MGK_ConfigureErrorRegistryKeyLookupFailed 87
#define MGK_ConfigureErrorUnableToAccessConfigureFile 88
#define MGK_ConfigureErrorUnableToAccessFontFile 89
#define MGK_ConfigureErrorUnableToAccessLogFile 90
#define MGK_ConfigureErrorUnableToAccessModuleFile 91
#define MGK_ConfigureFatalErrorDefault 92
#define MGK_ConfigureWarningDefault 93
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 94
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 95
#define MGK_CorruptImageErrorColormapExceeds256Colors 96
#define MGK_CorruptImageErrorCorruptImage 97
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 98
#define MGK_CorruptImageErrorImageTypeNotSupported 99
#define MGK_CorruptImageErrorImproperImageHeader 100
#define MGK_CorruptImageErrorInsufficientImageDataInFile 101
#define MGK_CorruptImageErrorInvalidColormapIndex 102
#define MGK_CorruptImageErrorInvalidFileFormatVersion 103
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 104
#define MGK_CorruptImageErrorMissingImageChannel 105
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 106
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 107
#define MGK_CorruptImageErrorNotEnoughTiles 108
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 109
#define MGK_CorruptImageErrorTooMuchImageDataInFile 110
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 111
#define MGK_CorruptImageErrorUnableToReadColorProfile 112
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 113
#define MGK_CorruptImageErrorUnableToReadGenericProfile 114
#define MGK_CorruptImageErrorUnableToReadImageData 115
#define MGK_CorruptImageErrorUnableToReadImageHeader 116
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 117
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 118
#define MGK_CorruptImageErrorUnableToReadSubImageData 119
#define MGK_CorruptImageErrorUnableToReadVIDImage 120
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 121
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 122
#define MGK_CorruptImageErrorUnableToUncompressImage 123
#define MGK_CorruptImageErrorUnexpectedEndOfFile 124
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 125
#define MGK_CorruptImageErrorUnknownPatternType 126
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 127
#define MGK_CorruptImageErrorUnrecognizedImageCompression 128
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 129
#define MGK_CorruptImageFatalErrorUnableToPersistKey 130
#define MGK_CorruptImageWarningCompressionNotValid 131
#define MGK_CorruptImageWarningImproperImageHeader 132
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 133
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 134
#define MGK_CorruptImageWarningSkipToSyncByte 135
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 136
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 137
#define MGK_CorruptImageWarningUnrecognizedImageCompression 138
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 139
#define MGK_DelegateErrorDelegateFailed 140
#define MGK_DelegateErrorFailedToComputeOutputSize 141
#define MGK_DelegateErrorFailedToRenderFile 142
#define MGK_DelegateErrorFailedToScanFile 143
#define MGK_DelegateErrorNoTagFound 144
#define MGK_DelegateErrorPostscriptDelegateFailed 145
#define MGK_DelegateErrorUnableToCreateImage 146
#define MGK_DelegateErrorUnableToCreateImageComponent 147
#define MGK_DelegateErrorUnableToDecodeImageFile 148
#define MGK_DelegateErrorUnableToEncodeImageFile 149
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 150
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 151
#define MGK_DelegateErrorUnableToManageJP2Stream 152
#define MGK_DelegateErrorUnableToWriteSVGFormat 153
#define MGK_DelegateFatalErrorDefault 154
#define MGK_DelegateWarningDefault 155
#define MGK_DrawErrorAlreadyPushingPatternDefinition 156
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 157
#define MGK_DrawErrorUnableToPrint 158
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 159
#define MGK_DrawFatalErrorDefault 160
#define MGK_DrawWarningNotARelativeURL 161
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 162
#define MGK_DrawWarningURLNotFound 163
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 164
#define MGK_FileOpenErrorUnableToOpenFile 165
#define MGK_FileOpenErrorUnableToWriteFile 166
#define MGK_FileOpenFatalErrorDefault 167
#define MGK_FileOpenWarningDefault 168
#define MGK_ImageErrorAngleIsDiscontinuous 169
#define MGK_ImageErrorColorspaceColorProfileMismatch 170
#define MGK_ImageErrorImageColorspaceDiffers 171
#define MGK_ImageErrorImageDoesNotContainResolution 172
#define MGK_ImageErrorImageOpacityDiffers 173
#define MGK_ImageErrorImageSequenceIsRequired 174
#define MGK_ImageErrorImageSizeDiffers 175
#define MGK_ImageErrorInvalidColormapIndex 176
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 177
#define MGK_ImageErrorNoImagesWereFound 178
#define MGK_ImageErrorNoImagesWereLoaded 179
#define MGK_ImageErrorNoLocaleImageAttribute 180
#define MGK_ImageErrorTooManyClusters 181
#define MGK_ImageErrorUnableToAppendImage 182
#define MGK_ImageErrorUnableToAssignProfile 183
#define MGK_ImageErrorUnableToAverageImage 184
#define MGK_ImageErrorUnableToCoalesceImage 185
#define MGK_ImageErrorUnableToCompareImages 186
#define MGK_ImageErrorUnableToCreateImageMosaic 187
#define MGK_ImageErrorUnableToCreateStereoImage 188
#define MGK_ImageErrorUnableToDeconstructImageSequence 189
#define MGK_ImageErrorUnableToFlattenImage 190
#define MGK_ImageErrorUnableToGetClipMask 191
#define MGK_ImageErrorUnableToResizeImage 192
#define MGK_ImageErrorUnableToSegmentImage 193
#define MGK_ImageErrorUnableToSetClipMask 194
#define MGK_ImageErrorUnableToShearImage 195
#define MGK_ImageErrorWidthOrHeightExceedsLimit 196
#define MGK_ImageFatalErrorUnableToPersistKey 197
#define MGK_ImageWarningDefault 198
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 199
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 200
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 201
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 202
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 203
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 204
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 205
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 206
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 207
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 208
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 209
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 210
#define MGK_MissingDelegateFatalErrorDefault 211
#define MGK_MissingDelegateWarningDefault 212
#define MGK_ModuleErrorFailedToCloseModule 213
#define MGK_ModuleErrorFailedToFindSymbol 214
#define MGK_ModuleErrorUnableToLoadModule 215
#define MGK_ModuleErrorUnableToRegisterImageFormat 216
#define MGK_ModuleErrorUnrecognizedModule 217
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 218
#define MGK_ModuleWarningDefault 219
#define MGK_OptionErrorBevelWidthIsNegative 220
#define MGK_OptionErrorColorSeparatedImageRequired 221
#define MGK_OptionErrorFrameIsLessThanImageSize 222
#define MGK_OptionErrorGeometryDimensionsAreZero 223
#define MGK_OptionErrorGeometryDoesNotContainImage 224
#define MGK_OptionErrorImagesAreNotTheSameSize 225
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 226
#define MGK_OptionErrorImageSmallerThanKernelWidth 227
#define MGK_OptionErrorImageSmallerThanRadius 228
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 229
#define MGK_OptionErrorInputImagesAlreadySpecified 230
#define MGK_OptionErrorKernelRadiusIsTooSmall 231
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 232
#define MGK_OptionErrorMissingAnImageFilename 233
#define MGK_OptionErrorMissingArgument 234
#define MGK_OptionErrorMustSpecifyAnImageName 235
#define MGK_OptionErrorMustSpecifyImageSize 236
#define MGK_OptionErrorNoBlobDefined 237
#define MGK_OptionErrorNoImagesDefined 238
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 239
#define MGK_OptionErrorNoProfileNameWasGiven 240
#define MGK_OptionErrorNullBlobArgument 241
#define MGK_OptionErrorReferenceImageRequired 242
#define MGK_OptionErrorReferenceIsNotMyType 243
#define MGK_OptionErrorSteganoImageRequired 244
#define MGK_OptionErrorStereoImageRequired 245
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 246
#define MGK_OptionErrorUnableToAddOrRemoveProfile 247
#define MGK_OptionErrorUnableToAllocateICCProfile 248
#define MGK_OptionErrorUnableToAverageImageSequence 249
#define MGK_OptionErrorUnableToBlurImage 250
#define MGK_OptionErrorUnableToChopImage 251
#define MGK_OptionErrorUnableToConstituteImage 252
#define MGK_OptionErrorUnableToConvolveImage 253
#define MGK_OptionErrorUnableToEdgeImage 254
#define MGK_OptionErrorUnableToEqualizeImage 255
#define MGK_OptionErrorUnableToFilterImage 256
#define MGK_OptionErrorUnableToFormatImageMetadata 257
#define MGK_OptionErrorUnableToFrameImage 258
#define MGK_OptionErrorUnableToOilPaintImage 259
#define MGK_OptionErrorUnableToPaintImage 260
#define MGK_OptionErrorUnableToRaiseImage 261
#define MGK_OptionErrorUnableToSharpenImage 262
#define MGK_OptionErrorUnableToThresholdImage 263
#define MGK_OptionErrorUnableToWaveImage 264
#define MGK_OptionErrorUnrecognizedAttribute 265
#define MGK_OptionErrorUnrecognizedChannelType 266
#define MGK_OptionErrorUnrecognizedColor 267
#define MGK_OptionErrorUnrecognizedColormapType 268
#define MGK_OptionErrorUnrecognizedColorspace 269
#define MGK_OptionErrorUnrecognizedCommand 270
#define MGK_OptionErrorUnrecognizedComposeOperator 271
#define MGK_OptionErrorUnrecognizedDisposeMethod 272
#define MGK_OptionErrorUnrecognizedElement 273
#define MGK_OptionErrorUnrecognizedEndianType 274
#define MGK_OptionErrorUnrecognizedGravityType 275
#define MGK_OptionErrorUnrecognizedImageCompression 276
#define MGK_OptionErrorUnrecognizedImageFilter 277
#define MGK_OptionErrorUnrecognizedImageFormat 278
#define MGK_OptionErrorUnrecognizedImageMode 279
#define MGK_OptionErrorUnrecognizedImageType 280
#define MGK_OptionErrorUnrecognizedIntentType 281
#define MGK_OptionErrorUnrecognizedInterlaceType 282
#define MGK_OptionErrorUnrecognizedListType 283
#define MGK_OptionErrorUnrecognizedModeType 284
#define MGK_OptionErrorUnrecognizedNoiseType 285
#define MGK_OptionErrorUnrecognizedOperator 286
#define MGK_OptionErrorUnrecognizedOption 287
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 288
#define MGK_OptionErrorUnrecognizedPixelMap 289
#define MGK_OptionErrorUnrecognizedPreviewType 290
#define MGK_OptionErrorUnrecognizedResourceType 291
#define MGK_OptionErrorUnrecognizedType 292
#define MGK_OptionErrorUnrecognizedUnitsType 293
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 294
#define MGK_OptionErrorUsageError 295
#define MGK_OptionFatalErrorInvalidColorspaceType 296
#define MGK_OptionFatalErrorInvalidEndianType 297
#define MGK_OptionFatalErrorInvalidImageType 298
#define MGK_OptionFatalErrorInvalidInterlaceType 299
#define MGK_OptionFatalErrorMissingAnImageFilename 300
#define MGK_OptionFatalErrorMissingArgument 301
#define MGK_OptionFatalErrorNoImagesWereLoaded 302
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 303
#define MGK_OptionFatalErrorUnableToOpenXServer 304
#define MGK_OptionFatalErrorUnableToPersistKey 305
#define MGK_OptionFatalErrorUnrecognizedColormapType 306
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 307
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 308
#define MGK_OptionFatalErrorUnrecognizedEndianType 309
#define MGK_OptionFatalErrorUnrecognizedFilterType 310
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 311
#define MGK_OptionFatalErrorUnrecognizedImageType 312
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 313
#define MGK_OptionFatalErrorUnrecognizedOption 314
#define MGK_OptionFatalErrorUnrecognizedResourceType 315
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 316
#define MGK_OptionWarningUnrecognizedColor 317
#define MGK_RegistryErrorImageExpected 318
#define MGK_RegistryErrorImageInfoExpected 319
#define MGK_RegistryErrorStructureSizeMismatch 320
#define MGK_RegistryErrorUnableToGetRegistryID 321
#define MGK_RegistryErrorUnableToLocateImage 322
#define MGK_RegistryErrorUnableToSetRegistry 323
#define MGK_RegistryFatalErrorDefault 324
#define MGK_RegistryWarningDefault 325
#define MGK_ResourceLimitErrorCacheResourcesExhausted 326
#define MGK_ResourceLimitErrorMemoryAllocationFailed 327
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 328
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 329
#define MGK_ResourceLimitErrorUnableToAddColorProfile 330
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 331
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 332
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 333
#define MGK_ResourceLimitErrorUnableToAllocateColormap 334
#define MGK_ResourceLimitErrorUnableToAllocateString 335
#define MGK_ResourceLimitErrorUnableToAnnotateImage 336
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 337
#define MGK_ResourceLimitErrorUnableToCloneImage 338
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 339
#define MGK_ResourceLimitErrorUnableToConstituteImage 340
#define MGK_ResourceLimitErrorUnableToConvertFont 341
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 342
#define MGK_ResourceLimitErrorUnableToCreateColormap 343
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 344
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 345
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 346
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 347
#define MGK_ResourceLimitErrorUnableToCreateXWindow 348
#define MGK_ResourceLimitErrorUnableToCropImage 349
#define MGK_ResourceLimitErrorUnableToDespeckleImage 350
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 351
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 352
#define MGK_ResourceLimitErrorUnableToDitherImage 353
#define MGK_ResourceLimitErrorUnableToDrawOnImage 354
#define MGK_ResourceLimitErrorUnableToEdgeImage 355
#define MGK_ResourceLimitErrorUnableToEmbossImage 356
#define MGK_ResourceLimitErrorUnableToEnhanceImage 357
#define MGK_ResourceLimitErrorUnableToFloodfillImage 358
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 359
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 360
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 361
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 362
#define MGK_ResourceLimitErrorUnableToLevelImage 363
#define MGK_ResourceLimitErrorUnableToMagnifyImage 364
#define MGK_ResourceLimitErrorUnableToManageColor 365
#define MGK_ResourceLimitErrorUnableToMapImage 366
#define MGK_ResourceLimitErrorUnableToMapImageSequence 367
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 368
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 369
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 370
#define MGK_ResourceLimitErrorUnableToNormalizeImage 371
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 372
#define MGK_ResourceLimitErrorUnableToQuantizeImage 373
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 374
#define MGK_ResourceLimitErrorUnableToReadTextChunk 375
#define MGK_ResourceLimitErrorUnableToReadXImage 376
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 377
#define MGK_ResourceLimitErrorUnableToResizeImage 378
#define MGK_ResourceLimitErrorUnableToRotateImage 379
#define MGK_ResourceLimitErrorUnableToSampleImage 380
#define MGK_ResourceLimitErrorUnableToScaleImage 381
#define MGK_ResourceLimitErrorUnableToSelectImage 382
#define MGK_ResourceLimitErrorUnableToSharpenImage 383
#define MGK_ResourceLimitErrorUnableToShaveImage 384
#define MGK_ResourceLimitErrorUnableToShearImage 385
#define MGK_ResourceLimitErrorUnableToSortImageColormap 386
#define MGK_ResourceLimitErrorUnableToThresholdImage 387
#define MGK_ResourceLimitErrorUnableToTransformColorspace 388
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 389
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 390
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 391
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 392
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 393
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 394
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 395
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 396
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 397
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 398
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 399
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 400
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 401
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 402
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 403
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 404
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 405
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 406
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 407
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 408
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 409
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 410
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 411
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 412
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 413
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 414
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 415
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 416
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 417
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 418
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 419
#define MGK_ResourceLimitFatalErrorUnableToConvertText 420
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 421
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 422
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 423
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 424
#define MGK_ResourceLimitWarningMemoryAllocationFailed 425
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 426
#define MGK_StreamErrorNoStreamHandlerIsDefined 427
#define MGK_StreamErrorPixelCacheIsNotOpen 428
#define MGK_StreamErrorUnableToAcquirePixelStream 429
#define MGK_StreamErrorUnableToSetPixelStream 430
#define MGK_StreamErrorUnableToSyncPixelStream 431
#define MGK_StreamFatalErrorDefault 432
#define MGK_StreamWarningDefault 433
#define MGK_TypeErrorFontSubstitutionRequired 434
#define MGK_TypeErrorUnableToGetTypeMetrics 435
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 436
#define MGK_TypeErrorUnableToReadFont 437
#define MGK_TypeErrorUnrecognizedFontEncoding 438
#define MGK_TypeFatalErrorDefault 439
#define MGK_TypeWarningDefault 440
#define MGK_WandErrorInvalidColormapIndex 441
#define MGK_WandErrorWandContainsNoImageIndexs 442
#define MGK_WandErrorWandContainsNoImages 443
#define MGK_XServerErrorColorIsNotKnownToServer 444
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 445
#define MGK_XServerErrorStandardColormapIsNotInitialized 446
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 447
#define MGK_XServerErrorUnableToCreateBitmap 448
#define MGK_XServerErrorUnableToCreateColormap 449
#define MGK_XServerErrorUnableToCreatePixmap 450
#define MGK_XServerErrorUnableToCreateProperty 451
#define MGK_XServerErrorUnableToCreateStandardColormap 452
#define MGK_XServerErrorUnableToDisplayImageInfo 453
#define MGK_XServerErrorUnableToGetProperty 454
#define MGK_XServerErrorUnableToGetStandardColormap 455
#define MGK_XServerErrorUnableToGetVisual 456
#define MGK_XServerErrorUnableToGrabMouse 457
#define MGK_XServerErrorUnableToLoadFont 458
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 459
#define MGK_XServerErrorUnableToOpenXServer 460
#define MGK_XServerErrorUnableToReadXAttributes 461
#define MGK_XServerErrorUnableToReadXWindowImage 462
#define MGK_XServerErrorUnrecognizedColormapType 463
#define MGK_XServerErrorUnrecognizedGravityType 464
#define MGK_XServerErrorUnrecognizedVisualSpecifier 465
#define MGK_XServerFatalErrorUnableToAllocateXHints 466
#define MGK_XServerFatalErrorUnableToCreateCursor 467
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 468
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 469
#define MGK_XServerFatalErrorUnableToCreateTextProperty 470
#define MGK_XServerFatalErrorUnableToCreateXImage 471
#define MGK_XServerFatalErrorUnableToCreateXPixmap 472
#define MGK_XServerFatalErrorUnableToCreateXWindow 473
#define MGK_XServerFatalErrorUnableToDisplayImage 474
#define MGK_XServerFatalErrorUnableToDitherImage 475
#define MGK_XServerFatalErrorUnableToGetPixelInfo 476
#define MGK_XServerFatalErrorUnableToGetVisual 477
#define MGK_XServerFatalErrorUnableToLoadFont 478
#define MGK_XServerFatalErrorUnableToMakeXWindow 479
#define MGK_XServerFatalErrorUnableToOpenXServer 480
#define MGK_XServerFatalErrorUnableToViewFonts 481
#define MGK_XServerWarningUnableToGetVisual 482
#define MGK_XServerWarningUsingDefaultVisual 483

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
    { "Wand", 48 },
    { "XServer", 49 },
    { 0, 51 }
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
    { "Coder/FatalError", 83, CoderFatalError },
    { "Coder/Warning", 84, CoderWarning },
    { "Configure/Error", 85, ConfigureError },
    { "Configure/FatalError", 91, ConfigureFatalError },
    { "Configure/Warning", 92, ConfigureWarning },
    { "Corrupt/Image/Error", 93, CorruptImageError },
    { "Corrupt/Image/FatalError", 129, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 130, CorruptImageWarning },
    { "Delegate/Error", 139, DelegateError },
    { "Delegate/FatalError", 153, DelegateFatalError },
    { "Delegate/Warning", 154, DelegateWarning },
    { "Draw/Error", 155, DrawError },
    { "Draw/FatalError", 159, DrawFatalError },
    { "Draw/Warning", 160, DrawWarning },
    { "File/Open/Error", 163, FileOpenError },
    { "File/Open/FatalError", 166, FileOpenFatalError },
    { "File/Open/Warning", 167, FileOpenWarning },
    { "Image/Error", 168, ImageError },
    { "Image/FatalError", 196, ImageFatalError },
    { "Image/Warning", 197, ImageWarning },
    { "Missing/Delegate/Error", 198, MissingDelegateError },
    { "Missing/Delegate/FatalError", 210, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 211, MissingDelegateWarning },
    { "Module/Error", 212, ModuleError },
    { "Module/FatalError", 217, ModuleFatalError },
    { "Module/Warning", 218, ModuleWarning },
    { "Option/Error", 219, OptionError },
    { "Option/FatalError", 295, OptionFatalError },
    { "Option/Warning", 316, OptionWarning },
    { "Registry/Error", 317, RegistryError },
    { "Registry/FatalError", 323, RegistryFatalError },
    { "Registry/Warning", 324, RegistryWarning },
    { "Resource/Limit/Error", 325, ResourceLimitError },
    { "Resource/Limit/FatalError", 388, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 424, ResourceLimitWarning },
    { "Stream/Error", 425, StreamError },
    { "Stream/FatalError", 431, StreamFatalError },
    { "Stream/Warning", 432, StreamWarning },
    { "Type/Error", 433, TypeError },
    { "Type/FatalError", 438, TypeFatalError },
    { "Type/Warning", 439, TypeWarning },
    { "Wand/Error", 440, WandError },
    { "XServer/Error", 443, XServerError },
    { "XServer/FatalError", 465, XServerFatalError },
    { "XServer/Warning", 481, XServerWarning },
    { 0, 483, UndefinedException }
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
    { "NumberOfImagesIsNotSupported", 54 },
    { "OnlyContinuousTonePictureSupported", 55 },
    { "OnlyLevelZerofilesSupported", 56 },
    { "PNGCompressionNotSupported", 57 },
    { "PNGLibraryTooOld", 58 },
    { "RLECompressionNotSupported", 59 },
    { "UnableToCopyProfile", 60 },
    { "UnableToCreateADC", 61 },
    { "UnableToCreateBitmap", 62 },
    { "UnableToDecompressImage", 63 },
    { "UnableToInitializeFPXLibrary", 64 },
    { "UnableToOpenBlob", 65 },
    { "UnableToReadAspectRatio", 66 },
    { "UnableToReadCIELABImages", 67 },
    { "UnableToReadSummaryInfo", 68 },
    { "UnableToSetAffineMatrix", 69 },
    { "UnableToSetAspectRatio", 70 },
    { "UnableToSetColorTwist", 71 },
    { "UnableToSetContrast", 72 },
    { "UnableToSetFilteringValue", 73 },
    { "UnableToSetImageComments", 74 },
    { "UnableToSetImageTitle", 75 },
    { "UnableToSetJPEGLevel", 76 },
    { "UnableToSetRegionOfInterest", 77 },
    { "UnableToSetSummaryInfo", 78 },
    { "UnableToTranslateText", 79 },
    { "UnableToWriteMPEGParameters", 80 },
    { "UnableToZipCompressImage", 81 },
    { "UnsupportedCellTypeInTheMatrix", 82 },
    { "ZipCompressionNotSupported", 83 },
    { "Default", 84 },
    { "LosslessToLossyJPEGConversion", 85 },
    { "IncludeElementNestedTooDeeply", 86 },
    { "RegistryKeyLookupFailed", 87 },
    { "UnableToAccessConfigureFile", 88 },
    { "UnableToAccessFontFile", 89 },
    { "UnableToAccessLogFile", 90 },
    { "UnableToAccessModuleFile", 91 },
    { "Default", 92 },
    { "Default", 93 },
    { "AnErrorHasOccurredReadingFromFile", 94 },
    { "AnErrorHasOccurredWritingToFile", 95 },
    { "ColormapExceeds256Colors", 96 },
    { "CorruptImage", 97 },
    { "ImageFileDoesNotContainAnyImageData", 98 },
    { "ImageTypeNotSupported", 99 },
    { "ImproperImageHeader", 100 },
    { "InsufficientImageDataInFile", 101 },
    { "InvalidColormapIndex", 102 },
    { "InvalidFileFormatVersion", 103 },
    { "LengthAndFilesizeDoNotMatch", 104 },
    { "MissingImageChannel", 105 },
    { "NegativeOrZeroImageSize", 106 },
    { "NonOS2HeaderSizeError", 107 },
    { "NotEnoughTiles", 108 },
    { "StaticPlanesValueNotEqualToOne", 109 },
    { "TooMuchImageDataInFile", 110 },
    { "UnableToReadColormapFromDumpFile", 111 },
    { "UnableToReadColorProfile", 112 },
    { "UnableToReadExtensionBlock", 113 },
    { "UnableToReadGenericProfile", 114 },
    { "UnableToReadImageData", 115 },
    { "UnableToReadImageHeader", 116 },
    { "UnableToReadIPTCProfile", 117 },
    { "UnableToReadPixmapFromDumpFile", 118 },
    { "UnableToReadSubImageData", 119 },
    { "UnableToReadVIDImage", 120 },
    { "UnableToReadWindowNameFromDumpFile", 121 },
    { "UnableToRunlengthDecodeImage", 122 },
    { "UnableToUncompressImage", 123 },
    { "UnexpectedEndOfFile", 124 },
    { "UnexpectedSamplingFactor", 125 },
    { "UnknownPatternType", 126 },
    { "UnrecognizedBitsPerPixel", 127 },
    { "UnrecognizedImageCompression", 128 },
    { "UnrecognizedXWDHeader", 129 },
    { "UnableToPersistKey", 130 },
    { "CompressionNotValid", 131 },
    { "ImproperImageHeader", 132 },
    { "NegativeOrZeroImageSize", 133 },
    { "NonOS2HeaderSizeError", 134 },
    { "SkipToSyncByte", 135 },
    { "StaticPlanesValueNotEqualToOne", 136 },
    { "UnrecognizedBitsPerPixel", 137 },
    { "UnrecognizedImageCompression", 138 },
    { "UnrecognizedNumberOfColors", 139 },
    { "DelegateFailed", 140 },
    { "FailedToComputeOutputSize", 141 },
    { "FailedToRenderFile", 142 },
    { "FailedToScanFile", 143 },
    { "NoTagFound", 144 },
    { "PostscriptDelegateFailed", 145 },
    { "UnableToCreateImage", 146 },
    { "UnableToCreateImageComponent", 147 },
    { "UnableToDecodeImageFile", 148 },
    { "UnableToEncodeImageFile", 149 },
    { "UnableToInitializeFPXLibrary", 150 },
    { "UnableToInitializeWMFLibrary", 151 },
    { "UnableToManageJP2Stream", 152 },
    { "UnableToWriteSVGFormat", 153 },
    { "Default", 154 },
    { "Default", 155 },
    { "AlreadyPushingPatternDefinition", 156 },
    { "NonconformingDrawingPrimitiveDefinition", 157 },
    { "UnableToPrint", 158 },
    { "UnbalancedGraphicContextPushPop", 159 },
    { "Default", 160 },
    { "NotARelativeURL", 161 },
    { "NotCurrentlyPushingPatternDefinition", 162 },
    { "URLNotFound", 163 },
    { "UnableToCreateTemporaryFile", 164 },
    { "UnableToOpenFile", 165 },
    { "UnableToWriteFile", 166 },
    { "Default", 167 },
    { "Default", 168 },
    { "AngleIsDiscontinuous", 169 },
    { "ColorspaceColorProfileMismatch", 170 },
    { "ImageColorspaceDiffers", 171 },
    { "ImageDoesNotContainResolution", 172 },
    { "ImageOpacityDiffers", 173 },
    { "ImageSequenceIsRequired", 174 },
    { "ImageSizeDiffers", 175 },
    { "InvalidColormapIndex", 176 },
    { "LeftAndRightImageSizesDiffer", 177 },
    { "NoImagesWereFound", 178 },
    { "NoImagesWereLoaded", 179 },
    { "NoLocaleImageAttribute", 180 },
    { "TooManyClusters", 181 },
    { "UnableToAppendImage", 182 },
    { "UnableToAssignProfile", 183 },
    { "UnableToAverageImage", 184 },
    { "UnableToCoalesceImage", 185 },
    { "UnableToCompareImages", 186 },
    { "UnableToCreateImageMosaic", 187 },
    { "UnableToCreateStereoImage", 188 },
    { "UnableToDeconstructImageSequence", 189 },
    { "UnableToFlattenImage", 190 },
    { "UnableToGetClipMask", 191 },
    { "UnableToResizeImage", 192 },
    { "UnableToSegmentImage", 193 },
    { "UnableToSetClipMask", 194 },
    { "UnableToShearImage", 195 },
    { "WidthOrHeightExceedsLimit", 196 },
    { "UnableToPersistKey", 197 },
    { "Default", 198 },
    { "DPSLibraryIsNotAvailable", 199 },
    { "FPXLibraryIsNotAvailable", 200 },
    { "FreeTypeLibraryIsNotAvailable", 201 },
    { "JPEGLibraryIsNotAvailable", 202 },
    { "LCMSLibraryIsNotAvailable", 203 },
    { "LZWEncodingNotEnabled", 204 },
    { "NoDecodeDelegateForThisImageFormat", 205 },
    { "NoEncodeDelegateForThisImageFormat", 206 },
    { "TIFFLibraryIsNotAvailable", 207 },
    { "XMLLibraryIsNotAvailable", 208 },
    { "XWindowLibraryIsNotAvailable", 209 },
    { "ZipLibraryIsNotAvailable", 210 },
    { "Default", 211 },
    { "Default", 212 },
    { "FailedToCloseModule", 213 },
    { "FailedToFindSymbol", 214 },
    { "UnableToLoadModule", 215 },
    { "UnableToRegisterImageFormat", 216 },
    { "UnrecognizedModule", 217 },
    { "UnableToInitializeModuleLoader", 218 },
    { "Default", 219 },
    { "BevelWidthIsNegative", 220 },
    { "ColorSeparatedImageRequired", 221 },
    { "FrameIsLessThanImageSize", 222 },
    { "GeometryDimensionsAreZero", 223 },
    { "GeometryDoesNotContainImage", 224 },
    { "ImagesAreNotTheSameSize", 225 },
    { "ImageSizeMustExceedBevelWidth", 226 },
    { "ImageSmallerThanKernelWidth", 227 },
    { "ImageSmallerThanRadius", 228 },
    { "ImageWidthsOrHeightsDiffer", 229 },
    { "InputImagesAlreadySpecified", 230 },
    { "KernelRadiusIsTooSmall", 231 },
    { "KernelWidthMustBeAnOddNumber", 232 },
    { "MissingAnImageFilename", 233 },
    { "MissingArgument", 234 },
    { "MustSpecifyAnImageName", 235 },
    { "MustSpecifyImageSize", 236 },
    { "NoBlobDefined", 237 },
    { "NoImagesDefined", 238 },
    { "NonzeroWidthAndHeightRequired", 239 },
    { "NoProfileNameWasGiven", 240 },
    { "NullBlobArgument", 241 },
    { "ReferenceImageRequired", 242 },
    { "ReferenceIsNotMyType", 243 },
    { "SteganoImageRequired", 244 },
    { "StereoImageRequired", 245 },
    { "SubimageSpecificationReturnsNoImages", 246 },
    { "UnableToAddOrRemoveProfile", 247 },
    { "UnableToAllocateICCProfile", 248 },
    { "UnableToAverageImageSequence", 249 },
    { "UnableToBlurImage", 250 },
    { "UnableToChopImage", 251 },
    { "UnableToConstituteImage", 252 },
    { "UnableToConvolveImage", 253 },
    { "UnableToEdgeImage", 254 },
    { "UnableToEqualizeImage", 255 },
    { "UnableToFilterImage", 256 },
    { "UnableToFormatImageMetadata", 257 },
    { "UnableToFrameImage", 258 },
    { "UnableToOilPaintImage", 259 },
    { "UnableToPaintImage", 260 },
    { "UnableToRaiseImage", 261 },
    { "UnableToSharpenImage", 262 },
    { "UnableToThresholdImage", 263 },
    { "UnableToWaveImage", 264 },
    { "UnrecognizedAttribute", 265 },
    { "UnrecognizedChannelType", 266 },
    { "UnrecognizedColor", 267 },
    { "UnrecognizedColormapType", 268 },
    { "UnrecognizedColorspace", 269 },
    { "UnrecognizedCommand", 270 },
    { "UnrecognizedComposeOperator", 271 },
    { "UnrecognizedDisposeMethod", 272 },
    { "UnrecognizedElement", 273 },
    { "UnrecognizedEndianType", 274 },
    { "UnrecognizedGravityType", 275 },
    { "UnrecognizedImageCompression", 276 },
    { "UnrecognizedImageFilter", 277 },
    { "UnrecognizedImageFormat", 278 },
    { "UnrecognizedImageMode", 279 },
    { "UnrecognizedImageType", 280 },
    { "UnrecognizedIntentType", 281 },
    { "UnrecognizedInterlaceType", 282 },
    { "UnrecognizedListType", 283 },
    { "UnrecognizedModeType", 284 },
    { "UnrecognizedNoiseType", 285 },
    { "UnrecognizedOperator", 286 },
    { "UnrecognizedOption", 287 },
    { "UnrecognizedPerlMagickMethod", 288 },
    { "UnrecognizedPixelMap", 289 },
    { "UnrecognizedPreviewType", 290 },
    { "UnrecognizedResourceType", 291 },
    { "UnrecognizedType", 292 },
    { "UnrecognizedUnitsType", 293 },
    { "UnrecognizedVirtualPixelMethod", 294 },
    { "UsageError", 295 },
    { "InvalidColorspaceType", 296 },
    { "InvalidEndianType", 297 },
    { "InvalidImageType", 298 },
    { "InvalidInterlaceType", 299 },
    { "MissingAnImageFilename", 300 },
    { "MissingArgument", 301 },
    { "NoImagesWereLoaded", 302 },
    { "OptionLengthExceedsLimit", 303 },
    { "UnableToOpenXServer", 304 },
    { "UnableToPersistKey", 305 },
    { "UnrecognizedColormapType", 306 },
    { "UnrecognizedColorspaceType", 307 },
    { "UnrecognizedDisposeMethod", 308 },
    { "UnrecognizedEndianType", 309 },
    { "UnrecognizedFilterType", 310 },
    { "UnrecognizedImageCompressionType", 311 },
    { "UnrecognizedImageType", 312 },
    { "UnrecognizedInterlaceType", 313 },
    { "UnrecognizedOption", 314 },
    { "UnrecognizedResourceType", 315 },
    { "UnrecognizedVirtualPixelMethod", 316 },
    { "UnrecognizedColor", 317 },
    { "ImageExpected", 318 },
    { "ImageInfoExpected", 319 },
    { "StructureSizeMismatch", 320 },
    { "UnableToGetRegistryID", 321 },
    { "UnableToLocateImage", 322 },
    { "UnableToSetRegistry", 323 },
    { "Default", 324 },
    { "Default", 325 },
    { "CacheResourcesExhausted", 326 },
    { "MemoryAllocationFailed", 327 },
    { "NoPixelsDefinedInCache", 328 },
    { "PixelCacheAllocationFailed", 329 },
    { "UnableToAddColorProfile", 330 },
    { "UnableToAddGenericProfile", 331 },
    { "UnableToAddIPTCProfile", 332 },
    { "UnableToAllocateCoefficients", 333 },
    { "UnableToAllocateColormap", 334 },
    { "UnableToAllocateString", 335 },
    { "UnableToAnnotateImage", 336 },
    { "UnableToAverageImageSequence", 337 },
    { "UnableToCloneImage", 338 },
    { "UnableToComputeImageSignature", 339 },
    { "UnableToConstituteImage", 340 },
    { "UnableToConvertFont", 341 },
    { "UnableToConvertStringToTokens", 342 },
    { "UnableToCreateColormap", 343 },
    { "UnableToCreateColorTransform", 344 },
    { "UnableToCreateCommandWidget", 345 },
    { "UnableToCreateImageGroup", 346 },
    { "UnableToCreateImageMontage", 347 },
    { "UnableToCreateXWindow", 348 },
    { "UnableToCropImage", 349 },
    { "UnableToDespeckleImage", 350 },
    { "UnableToDetermineImageClass", 351 },
    { "UnableToDetermineTheNumberOfImageColors", 352 },
    { "UnableToDitherImage", 353 },
    { "UnableToDrawOnImage", 354 },
    { "UnableToEdgeImage", 355 },
    { "UnableToEmbossImage", 356 },
    { "UnableToEnhanceImage", 357 },
    { "UnableToFloodfillImage", 358 },
    { "UnableToGammaCorrectImage", 359 },
    { "UnableToGetBestIconSize", 360 },
    { "UnableToGetFromRegistry", 361 },
    { "UnableToGetPackageInfo", 362 },
    { "UnableToLevelImage", 363 },
    { "UnableToMagnifyImage", 364 },
    { "UnableToManageColor", 365 },
    { "UnableToMapImage", 366 },
    { "UnableToMapImageSequence", 367 },
    { "UnableToMedianFilterImage", 368 },
    { "UnableToMotionBlurImage", 369 },
    { "UnableToNoiseFilterImage", 370 },
    { "UnableToNormalizeImage", 371 },
    { "UnableToOpenColorProfile", 372 },
    { "UnableToQuantizeImage", 373 },
    { "UnableToQuantizeImageSequence", 374 },
    { "UnableToReadTextChunk", 375 },
    { "UnableToReadXImage", 376 },
    { "UnableToReadXServerColormap", 377 },
    { "UnableToResizeImage", 378 },
    { "UnableToRotateImage", 379 },
    { "UnableToSampleImage", 380 },
    { "UnableToScaleImage", 381 },
    { "UnableToSelectImage", 382 },
    { "UnableToSharpenImage", 383 },
    { "UnableToShaveImage", 384 },
    { "UnableToShearImage", 385 },
    { "UnableToSortImageColormap", 386 },
    { "UnableToThresholdImage", 387 },
    { "UnableToTransformColorspace", 388 },
    { "MemoryAllocationFailed", 389 },
    { "UnableToAllocateAscii85Info", 390 },
    { "UnableToAllocateCacheInfo", 391 },
    { "UnableToAllocateCacheView", 392 },
    { "UnableToAllocateColorInfo", 393 },
    { "UnableToAllocateDashPattern", 394 },
    { "UnableToAllocateDelegateInfo", 395 },
    { "UnableToAllocateDerivatives", 396 },
    { "UnableToAllocateDrawContext", 397 },
    { "UnableToAllocateDrawInfo", 398 },
    { "UnableToAllocateDrawingWand", 399 },
    { "UnableToAllocateGammaMap", 400 },
    { "UnableToAllocateImage", 401 },
    { "UnableToAllocateImagePixels", 402 },
    { "UnableToAllocateLogInfo", 403 },
    { "UnableToAllocateMagicInfo", 404 },
    { "UnableToAllocateMagickInfo", 405 },
    { "UnableToAllocateModuleInfo", 406 },
    { "UnableToAllocateMontageInfo", 407 },
    { "UnableToAllocateQuantizeInfo", 408 },
    { "UnableToAllocateRegistryInfo", 409 },
    { "UnableToAllocateSemaphoreInfo", 410 },
    { "UnableToAllocateString", 411 },
    { "UnableToAllocateTypeInfo", 412 },
    { "UnableToAllocateWand", 413 },
    { "UnableToAnimateImageSequence", 414 },
    { "UnableToCloneBlobInfo", 415 },
    { "UnableToCloneCacheInfo", 416 },
    { "UnableToCloneImage", 417 },
    { "UnableToCloneImageInfo", 418 },
    { "UnableToConcatenateString", 419 },
    { "UnableToConvertText", 420 },
    { "UnableToCreateColormap", 421 },
    { "UnableToDisplayImage", 422 },
    { "UnableToEscapeString", 423 },
    { "UnableToInterpretMSLImage", 424 },
    { "MemoryAllocationFailed", 425 },
    { "ImageDoesNotContainTheStreamGeometry", 426 },
    { "NoStreamHandlerIsDefined", 427 },
    { "PixelCacheIsNotOpen", 428 },
    { "UnableToAcquirePixelStream", 429 },
    { "UnableToSetPixelStream", 430 },
    { "UnableToSyncPixelStream", 431 },
    { "Default", 432 },
    { "Default", 433 },
    { "FontSubstitutionRequired", 434 },
    { "UnableToGetTypeMetrics", 435 },
    { "UnableToInitializeFreetypeLibrary", 436 },
    { "UnableToReadFont", 437 },
    { "UnrecognizedFontEncoding", 438 },
    { "Default", 439 },
    { "Default", 440 },
    { "InvalidColormapIndex", 441 },
    { "WandContainsNoImageIndexs", 442 },
    { "WandContainsNoImages", 443 },
    { "ColorIsNotKnownToServer", 444 },
    { "NoWindowWithSpecifiedIDExists", 445 },
    { "StandardColormapIsNotInitialized", 446 },
    { "UnableToConnectToRemoteDisplay", 447 },
    { "UnableToCreateBitmap", 448 },
    { "UnableToCreateColormap", 449 },
    { "UnableToCreatePixmap", 450 },
    { "UnableToCreateProperty", 451 },
    { "UnableToCreateStandardColormap", 452 },
    { "UnableToDisplayImageInfo", 453 },
    { "UnableToGetProperty", 454 },
    { "UnableToGetStandardColormap", 455 },
    { "UnableToGetVisual", 456 },
    { "UnableToGrabMouse", 457 },
    { "UnableToLoadFont", 458 },
    { "UnableToMatchVisualToStandardColormap", 459 },
    { "UnableToOpenXServer", 460 },
    { "UnableToReadXAttributes", 461 },
    { "UnableToReadXWindowImage", 462 },
    { "UnrecognizedColormapType", 463 },
    { "UnrecognizedGravityType", 464 },
    { "UnrecognizedVisualSpecifier", 465 },
    { "UnableToAllocateXHints", 466 },
    { "UnableToCreateCursor", 467 },
    { "UnableToCreateGraphicContext", 468 },
    { "UnableToCreateStandardColormap", 469 },
    { "UnableToCreateTextProperty", 470 },
    { "UnableToCreateXImage", 471 },
    { "UnableToCreateXPixmap", 472 },
    { "UnableToCreateXWindow", 473 },
    { "UnableToDisplayImage", 474 },
    { "UnableToDitherImage", 475 },
    { "UnableToGetPixelInfo", 476 },
    { "UnableToGetVisual", 477 },
    { "UnableToLoadFont", 478 },
    { "UnableToMakeXWindow", 479 },
    { "UnableToOpenXServer", 480 },
    { "UnableToViewFonts", 481 },
    { "UnableToGetVisual", 482 },
    { "UsingDefaultVisual", 483 },
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
    "image does not contain resolution",
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
    "Unable to get clip mask",
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
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
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
    "Unrecognized command '%s'. Use -help for a usage summary or see manual.",
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
    "Unrecognized operator",
    "Unrecognized option",
    "Unrecognized PerlMagick method",
    "Unrecognized pixel map",
    "Unrecognized preview type",
    "Unrecognized resource type",
    "Unrecognized type",
    "Unrecognized units type",
    "Unrecognized virtual pixel method",
    "Improper arguments supplied, please see manual",
    "Invalid colorspace type",
    "Invalid endian type",
    "Invalid image type",
    "Invalid interlace type",
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
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
    "unable to allocate wand '%s",
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
    "invalid colormap index `%.1024s",
    "Wand contains no image indices `%.1024s",
    "Wand contains no images `%.1024s",
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
