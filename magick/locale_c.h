#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 485

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
#define MGK_CoderErrorColormapTooLarge 23
#define MGK_CoderErrorColormapTypeNotSupported 24
#define MGK_CoderErrorColorspaceModelIsNotSupported 25
#define MGK_CoderErrorColorTypeNotSupported 26
#define MGK_CoderErrorCompressionNotValid 27
#define MGK_CoderErrorDataEncodingSchemeIsNotSupported 28
#define MGK_CoderErrorDataStorageTypeIsNotSupported 29
#define MGK_CoderErrorDeltaPNGNotSupported 30
#define MGK_CoderErrorEncryptedWPGImageFileNotSupported 31
#define MGK_CoderErrorFractalCompressionNotSupported 32
#define MGK_CoderErrorImageColumnOrRowSizeIsNotSupported 33
#define MGK_CoderErrorImageDoesNotHaveAMatteChannel 34
#define MGK_CoderErrorImageIsNotTiled 35
#define MGK_CoderErrorImageTypeNotSupported 36
#define MGK_CoderErrorIncompatibleSizeOfDouble 37
#define MGK_CoderErrorIrregularChannelGeometryNotSupported 38
#define MGK_CoderErrorJNGCompressionNotSupported 39
#define MGK_CoderErrorJPEGCompressionNotSupported 40
#define MGK_CoderErrorJPEGEmbeddingFailed 41
#define MGK_CoderErrorLocationTypeIsNotSupported 42
#define MGK_CoderErrorMapStorageTypeIsNotSupported 43
#define MGK_CoderErrorMSBByteOrderNotSupported 44
#define MGK_CoderErrorMultidimensionalMatricesAreNotSupported 45
#define MGK_CoderErrorMultipleRecordListNotSupported 46
#define MGK_CoderErrorNo8BIMDataIsAvailable 47
#define MGK_CoderErrorNoAPP1DataIsAvailable 48
#define MGK_CoderErrorNoBitmapOnClipboard 49
#define MGK_CoderErrorNoColorProfileAvailable 50
#define MGK_CoderErrorNoDataReturned 51
#define MGK_CoderErrorNoImageVectorGraphics 52
#define MGK_CoderErrorNoIPTCInfoWasFound 53
#define MGK_CoderErrorNoIPTCProfileAvailable 54
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
#define MGK_CorruptImageErrorNotEnoughTiles 109
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 110
#define MGK_CorruptImageErrorTooMuchImageDataInFile 111
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 112
#define MGK_CorruptImageErrorUnableToReadColorProfile 113
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 114
#define MGK_CorruptImageErrorUnableToReadGenericProfile 115
#define MGK_CorruptImageErrorUnableToReadImageData 116
#define MGK_CorruptImageErrorUnableToReadImageHeader 117
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 118
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 119
#define MGK_CorruptImageErrorUnableToReadSubImageData 120
#define MGK_CorruptImageErrorUnableToReadVIDImage 121
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 122
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 123
#define MGK_CorruptImageErrorUnableToUncompressImage 124
#define MGK_CorruptImageErrorUnexpectedEndOfFile 125
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 126
#define MGK_CorruptImageErrorUnknownPatternType 127
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 128
#define MGK_CorruptImageErrorUnrecognizedImageCompression 129
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 130
#define MGK_CorruptImageFatalErrorUnableToPersistKey 131
#define MGK_CorruptImageWarningCompressionNotValid 132
#define MGK_CorruptImageWarningImproperImageHeader 133
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 134
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 135
#define MGK_CorruptImageWarningSkipToSyncByte 136
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 137
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 138
#define MGK_CorruptImageWarningUnrecognizedImageCompression 139
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 140
#define MGK_DelegateErrorDelegateFailed 141
#define MGK_DelegateErrorFailedToComputeOutputSize 142
#define MGK_DelegateErrorFailedToRenderFile 143
#define MGK_DelegateErrorFailedToScanFile 144
#define MGK_DelegateErrorNoTagFound 145
#define MGK_DelegateErrorPostscriptDelegateFailed 146
#define MGK_DelegateErrorUnableToCreateImage 147
#define MGK_DelegateErrorUnableToCreateImageComponent 148
#define MGK_DelegateErrorUnableToDecodeImageFile 149
#define MGK_DelegateErrorUnableToEncodeImageFile 150
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 151
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 152
#define MGK_DelegateErrorUnableToManageJP2Stream 153
#define MGK_DelegateErrorUnableToWriteSVGFormat 154
#define MGK_DelegateFatalErrorDefault 155
#define MGK_DelegateWarningDefault 156
#define MGK_DrawErrorAlreadyPushingPatternDefinition 157
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 158
#define MGK_DrawErrorUnableToPrint 159
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 160
#define MGK_DrawFatalErrorDefault 161
#define MGK_DrawWarningNotARelativeURL 162
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 163
#define MGK_DrawWarningURLNotFound 164
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 165
#define MGK_FileOpenErrorUnableToOpenFile 166
#define MGK_FileOpenErrorUnableToWriteFile 167
#define MGK_FileOpenFatalErrorDefault 168
#define MGK_FileOpenWarningDefault 169
#define MGK_ImageErrorAngleIsDiscontinuous 170
#define MGK_ImageErrorColorspaceColorProfileMismatch 171
#define MGK_ImageErrorImageColorspaceDiffers 172
#define MGK_ImageErrorImageDoesNotContainResolution 173
#define MGK_ImageErrorImageOpacityDiffers 174
#define MGK_ImageErrorImageSequenceIsRequired 175
#define MGK_ImageErrorImageSizeDiffers 176
#define MGK_ImageErrorInvalidColormapIndex 177
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 178
#define MGK_ImageErrorNoImagesWereFound 179
#define MGK_ImageErrorNoImagesWereLoaded 180
#define MGK_ImageErrorNoLocaleImageAttribute 181
#define MGK_ImageErrorTooManyClusters 182
#define MGK_ImageErrorUnableToAppendImage 183
#define MGK_ImageErrorUnableToAssignProfile 184
#define MGK_ImageErrorUnableToAverageImage 185
#define MGK_ImageErrorUnableToCoalesceImage 186
#define MGK_ImageErrorUnableToCompareImages 187
#define MGK_ImageErrorUnableToCreateImageMosaic 188
#define MGK_ImageErrorUnableToCreateStereoImage 189
#define MGK_ImageErrorUnableToDeconstructImageSequence 190
#define MGK_ImageErrorUnableToFlattenImage 191
#define MGK_ImageErrorUnableToGetClipMask 192
#define MGK_ImageErrorUnableToResizeImage 193
#define MGK_ImageErrorUnableToSegmentImage 194
#define MGK_ImageErrorUnableToSetClipMask 195
#define MGK_ImageErrorUnableToShearImage 196
#define MGK_ImageErrorWidthOrHeightExceedsLimit 197
#define MGK_ImageFatalErrorUnableToPersistKey 198
#define MGK_ImageWarningDefault 199
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 200
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 201
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 202
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 203
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 204
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 205
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 206
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 207
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 208
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 209
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 210
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 211
#define MGK_MissingDelegateFatalErrorDefault 212
#define MGK_MissingDelegateWarningDefault 213
#define MGK_ModuleErrorFailedToCloseModule 214
#define MGK_ModuleErrorFailedToFindSymbol 215
#define MGK_ModuleErrorUnableToLoadModule 216
#define MGK_ModuleErrorUnableToRegisterImageFormat 217
#define MGK_ModuleErrorUnrecognizedModule 218
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 219
#define MGK_ModuleWarningDefault 220
#define MGK_OptionErrorBevelWidthIsNegative 221
#define MGK_OptionErrorColorSeparatedImageRequired 222
#define MGK_OptionErrorFrameIsLessThanImageSize 223
#define MGK_OptionErrorGeometryDimensionsAreZero 224
#define MGK_OptionErrorGeometryDoesNotContainImage 225
#define MGK_OptionErrorImagesAreNotTheSameSize 226
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 227
#define MGK_OptionErrorImageSmallerThanKernelWidth 228
#define MGK_OptionErrorImageSmallerThanRadius 229
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 230
#define MGK_OptionErrorInputImagesAlreadySpecified 231
#define MGK_OptionErrorKernelRadiusIsTooSmall 232
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 233
#define MGK_OptionErrorMissingAnImageFilename 234
#define MGK_OptionErrorMissingArgument 235
#define MGK_OptionErrorMustSpecifyAnImageName 236
#define MGK_OptionErrorMustSpecifyImageSize 237
#define MGK_OptionErrorNoBlobDefined 238
#define MGK_OptionErrorNoImagesDefined 239
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 240
#define MGK_OptionErrorNoProfileNameWasGiven 241
#define MGK_OptionErrorNullBlobArgument 242
#define MGK_OptionErrorReferenceImageRequired 243
#define MGK_OptionErrorReferenceIsNotMyType 244
#define MGK_OptionErrorSteganoImageRequired 245
#define MGK_OptionErrorStereoImageRequired 246
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 247
#define MGK_OptionErrorUnableToAddOrRemoveProfile 248
#define MGK_OptionErrorUnableToAllocateICCProfile 249
#define MGK_OptionErrorUnableToAverageImageSequence 250
#define MGK_OptionErrorUnableToBlurImage 251
#define MGK_OptionErrorUnableToChopImage 252
#define MGK_OptionErrorUnableToConstituteImage 253
#define MGK_OptionErrorUnableToConvolveImage 254
#define MGK_OptionErrorUnableToEdgeImage 255
#define MGK_OptionErrorUnableToEqualizeImage 256
#define MGK_OptionErrorUnableToFilterImage 257
#define MGK_OptionErrorUnableToFormatImageMetadata 258
#define MGK_OptionErrorUnableToFrameImage 259
#define MGK_OptionErrorUnableToOilPaintImage 260
#define MGK_OptionErrorUnableToPaintImage 261
#define MGK_OptionErrorUnableToRaiseImage 262
#define MGK_OptionErrorUnableToSharpenImage 263
#define MGK_OptionErrorUnableToThresholdImage 264
#define MGK_OptionErrorUnableToWaveImage 265
#define MGK_OptionErrorUnrecognizedAttribute 266
#define MGK_OptionErrorUnrecognizedChannelType 267
#define MGK_OptionErrorUnrecognizedColor 268
#define MGK_OptionErrorUnrecognizedColormapType 269
#define MGK_OptionErrorUnrecognizedColorspace 270
#define MGK_OptionErrorUnrecognizedCommand 271
#define MGK_OptionErrorUnrecognizedComposeOperator 272
#define MGK_OptionErrorUnrecognizedDisposeMethod 273
#define MGK_OptionErrorUnrecognizedElement 274
#define MGK_OptionErrorUnrecognizedEndianType 275
#define MGK_OptionErrorUnrecognizedGravityType 276
#define MGK_OptionErrorUnrecognizedImageCompression 277
#define MGK_OptionErrorUnrecognizedImageFilter 278
#define MGK_OptionErrorUnrecognizedImageFormat 279
#define MGK_OptionErrorUnrecognizedImageMode 280
#define MGK_OptionErrorUnrecognizedImageType 281
#define MGK_OptionErrorUnrecognizedIntentType 282
#define MGK_OptionErrorUnrecognizedInterlaceType 283
#define MGK_OptionErrorUnrecognizedListType 284
#define MGK_OptionErrorUnrecognizedModeType 285
#define MGK_OptionErrorUnrecognizedNoiseType 286
#define MGK_OptionErrorUnrecognizedOperator 287
#define MGK_OptionErrorUnrecognizedOption 288
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 289
#define MGK_OptionErrorUnrecognizedPixelMap 290
#define MGK_OptionErrorUnrecognizedPreviewType 291
#define MGK_OptionErrorUnrecognizedResourceType 292
#define MGK_OptionErrorUnrecognizedType 293
#define MGK_OptionErrorUnrecognizedUnitsType 294
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 295
#define MGK_OptionErrorUsageError 296
#define MGK_OptionFatalErrorInvalidColorspaceType 297
#define MGK_OptionFatalErrorInvalidEndianType 298
#define MGK_OptionFatalErrorInvalidImageType 299
#define MGK_OptionFatalErrorInvalidInterlaceType 300
#define MGK_OptionFatalErrorMissingAnImageFilename 301
#define MGK_OptionFatalErrorMissingArgument 302
#define MGK_OptionFatalErrorNoImagesWereLoaded 303
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 304
#define MGK_OptionFatalErrorUnableToOpenXServer 305
#define MGK_OptionFatalErrorUnableToPersistKey 306
#define MGK_OptionFatalErrorUnrecognizedColormapType 307
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 308
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 309
#define MGK_OptionFatalErrorUnrecognizedEndianType 310
#define MGK_OptionFatalErrorUnrecognizedFilterType 311
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 312
#define MGK_OptionFatalErrorUnrecognizedImageType 313
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 314
#define MGK_OptionFatalErrorUnrecognizedOption 315
#define MGK_OptionFatalErrorUnrecognizedResourceType 316
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 317
#define MGK_OptionWarningUnrecognizedColor 318
#define MGK_RegistryErrorImageExpected 319
#define MGK_RegistryErrorImageInfoExpected 320
#define MGK_RegistryErrorStructureSizeMismatch 321
#define MGK_RegistryErrorUnableToGetRegistryID 322
#define MGK_RegistryErrorUnableToLocateImage 323
#define MGK_RegistryErrorUnableToSetRegistry 324
#define MGK_RegistryFatalErrorDefault 325
#define MGK_RegistryWarningDefault 326
#define MGK_ResourceLimitErrorCacheResourcesExhausted 327
#define MGK_ResourceLimitErrorMemoryAllocationFailed 328
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 329
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 330
#define MGK_ResourceLimitErrorUnableToAddColorProfile 331
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 332
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 333
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 334
#define MGK_ResourceLimitErrorUnableToAllocateColormap 335
#define MGK_ResourceLimitErrorUnableToAllocateString 336
#define MGK_ResourceLimitErrorUnableToAnnotateImage 337
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 338
#define MGK_ResourceLimitErrorUnableToCloneImage 339
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 340
#define MGK_ResourceLimitErrorUnableToConstituteImage 341
#define MGK_ResourceLimitErrorUnableToConvertFont 342
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 343
#define MGK_ResourceLimitErrorUnableToCreateColormap 344
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 345
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 346
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 347
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 348
#define MGK_ResourceLimitErrorUnableToCreateXWindow 349
#define MGK_ResourceLimitErrorUnableToCropImage 350
#define MGK_ResourceLimitErrorUnableToDespeckleImage 351
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 352
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 353
#define MGK_ResourceLimitErrorUnableToDitherImage 354
#define MGK_ResourceLimitErrorUnableToDrawOnImage 355
#define MGK_ResourceLimitErrorUnableToEdgeImage 356
#define MGK_ResourceLimitErrorUnableToEmbossImage 357
#define MGK_ResourceLimitErrorUnableToEnhanceImage 358
#define MGK_ResourceLimitErrorUnableToFloodfillImage 359
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 360
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 361
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 362
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 363
#define MGK_ResourceLimitErrorUnableToLevelImage 364
#define MGK_ResourceLimitErrorUnableToMagnifyImage 365
#define MGK_ResourceLimitErrorUnableToManageColor 366
#define MGK_ResourceLimitErrorUnableToMapImage 367
#define MGK_ResourceLimitErrorUnableToMapImageSequence 368
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 369
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 370
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 371
#define MGK_ResourceLimitErrorUnableToNormalizeImage 372
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 373
#define MGK_ResourceLimitErrorUnableToQuantizeImage 374
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 375
#define MGK_ResourceLimitErrorUnableToReadTextChunk 376
#define MGK_ResourceLimitErrorUnableToReadXImage 377
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 378
#define MGK_ResourceLimitErrorUnableToResizeImage 379
#define MGK_ResourceLimitErrorUnableToRotateImage 380
#define MGK_ResourceLimitErrorUnableToSampleImage 381
#define MGK_ResourceLimitErrorUnableToScaleImage 382
#define MGK_ResourceLimitErrorUnableToSelectImage 383
#define MGK_ResourceLimitErrorUnableToSharpenImage 384
#define MGK_ResourceLimitErrorUnableToShaveImage 385
#define MGK_ResourceLimitErrorUnableToShearImage 386
#define MGK_ResourceLimitErrorUnableToSortImageColormap 387
#define MGK_ResourceLimitErrorUnableToThresholdImage 388
#define MGK_ResourceLimitErrorUnableToTransformColorspace 389
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 390
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 391
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 392
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 393
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 394
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 395
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 396
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 397
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 398
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 399
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 400
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 401
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 402
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 403
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 404
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 405
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 406
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 407
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 408
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 409
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 410
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 411
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 412
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 413
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 414
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 415
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 416
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 417
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 418
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 419
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 420
#define MGK_ResourceLimitFatalErrorUnableToConvertText 421
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 422
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 423
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 424
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 425
#define MGK_ResourceLimitWarningMemoryAllocationFailed 426
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 427
#define MGK_StreamErrorNoStreamHandlerIsDefined 428
#define MGK_StreamErrorPixelCacheIsNotOpen 429
#define MGK_StreamErrorUnableToAcquirePixelStream 430
#define MGK_StreamErrorUnableToSetPixelStream 431
#define MGK_StreamErrorUnableToSyncPixelStream 432
#define MGK_StreamFatalErrorDefault 433
#define MGK_StreamWarningDefault 434
#define MGK_TypeErrorFontSubstitutionRequired 435
#define MGK_TypeErrorUnableToGetTypeMetrics 436
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 437
#define MGK_TypeErrorUnableToReadFont 438
#define MGK_TypeErrorUnrecognizedFontEncoding 439
#define MGK_TypeFatalErrorDefault 440
#define MGK_TypeWarningDefault 441
#define MGK_WandErrorInvalidColormapIndex 442
#define MGK_WandErrorWandAPINotImplemented 443
#define MGK_WandErrorWandContainsNoImageIndexs 444
#define MGK_WandErrorWandContainsNoImages 445
#define MGK_XServerErrorColorIsNotKnownToServer 446
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 447
#define MGK_XServerErrorStandardColormapIsNotInitialized 448
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 449
#define MGK_XServerErrorUnableToCreateBitmap 450
#define MGK_XServerErrorUnableToCreateColormap 451
#define MGK_XServerErrorUnableToCreatePixmap 452
#define MGK_XServerErrorUnableToCreateProperty 453
#define MGK_XServerErrorUnableToCreateStandardColormap 454
#define MGK_XServerErrorUnableToDisplayImageInfo 455
#define MGK_XServerErrorUnableToGetProperty 456
#define MGK_XServerErrorUnableToGetStandardColormap 457
#define MGK_XServerErrorUnableToGetVisual 458
#define MGK_XServerErrorUnableToGrabMouse 459
#define MGK_XServerErrorUnableToLoadFont 460
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 461
#define MGK_XServerErrorUnableToOpenXServer 462
#define MGK_XServerErrorUnableToReadXAttributes 463
#define MGK_XServerErrorUnableToReadXWindowImage 464
#define MGK_XServerErrorUnrecognizedColormapType 465
#define MGK_XServerErrorUnrecognizedGravityType 466
#define MGK_XServerErrorUnrecognizedVisualSpecifier 467
#define MGK_XServerFatalErrorUnableToAllocateXHints 468
#define MGK_XServerFatalErrorUnableToCreateCursor 469
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 470
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 471
#define MGK_XServerFatalErrorUnableToCreateTextProperty 472
#define MGK_XServerFatalErrorUnableToCreateXImage 473
#define MGK_XServerFatalErrorUnableToCreateXPixmap 474
#define MGK_XServerFatalErrorUnableToCreateXWindow 475
#define MGK_XServerFatalErrorUnableToDisplayImage 476
#define MGK_XServerFatalErrorUnableToDitherImage 477
#define MGK_XServerFatalErrorUnableToGetPixelInfo 478
#define MGK_XServerFatalErrorUnableToGetVisual 479
#define MGK_XServerFatalErrorUnableToLoadFont 480
#define MGK_XServerFatalErrorUnableToMakeXWindow 481
#define MGK_XServerFatalErrorUnableToOpenXServer 482
#define MGK_XServerFatalErrorUnableToViewFonts 483
#define MGK_XServerWarningUnableToGetVisual 484
#define MGK_XServerWarningUsingDefaultVisual 485

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
    { "Coder/FatalError", 84, CoderFatalError },
    { "Coder/Warning", 85, CoderWarning },
    { "Configure/Error", 86, ConfigureError },
    { "Configure/FatalError", 92, ConfigureFatalError },
    { "Configure/Warning", 93, ConfigureWarning },
    { "Corrupt/Image/Error", 94, CorruptImageError },
    { "Corrupt/Image/FatalError", 130, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 131, CorruptImageWarning },
    { "Delegate/Error", 140, DelegateError },
    { "Delegate/FatalError", 154, DelegateFatalError },
    { "Delegate/Warning", 155, DelegateWarning },
    { "Draw/Error", 156, DrawError },
    { "Draw/FatalError", 160, DrawFatalError },
    { "Draw/Warning", 161, DrawWarning },
    { "File/Open/Error", 164, FileOpenError },
    { "File/Open/FatalError", 167, FileOpenFatalError },
    { "File/Open/Warning", 168, FileOpenWarning },
    { "Image/Error", 169, ImageError },
    { "Image/FatalError", 197, ImageFatalError },
    { "Image/Warning", 198, ImageWarning },
    { "Missing/Delegate/Error", 199, MissingDelegateError },
    { "Missing/Delegate/FatalError", 211, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 212, MissingDelegateWarning },
    { "Module/Error", 213, ModuleError },
    { "Module/FatalError", 218, ModuleFatalError },
    { "Module/Warning", 219, ModuleWarning },
    { "Option/Error", 220, OptionError },
    { "Option/FatalError", 296, OptionFatalError },
    { "Option/Warning", 317, OptionWarning },
    { "Registry/Error", 318, RegistryError },
    { "Registry/FatalError", 324, RegistryFatalError },
    { "Registry/Warning", 325, RegistryWarning },
    { "Resource/Limit/Error", 326, ResourceLimitError },
    { "Resource/Limit/FatalError", 389, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 425, ResourceLimitWarning },
    { "Stream/Error", 426, StreamError },
    { "Stream/FatalError", 432, StreamFatalError },
    { "Stream/Warning", 433, StreamWarning },
    { "Type/Error", 434, TypeError },
    { "Type/FatalError", 439, TypeFatalError },
    { "Type/Warning", 440, TypeWarning },
    { "Wand/Error", 441, WandError },
    { "XServer/Error", 445, XServerError },
    { "XServer/FatalError", 467, XServerFatalError },
    { "XServer/Warning", 483, XServerWarning },
    { 0, 485, UndefinedException }
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
    { "ColormapTooLarge", 23 },
    { "ColormapTypeNotSupported", 24 },
    { "ColorspaceModelIsNotSupported", 25 },
    { "ColorTypeNotSupported", 26 },
    { "CompressionNotValid", 27 },
    { "DataEncodingSchemeIsNotSupported", 28 },
    { "DataStorageTypeIsNotSupported", 29 },
    { "DeltaPNGNotSupported", 30 },
    { "EncryptedWPGImageFileNotSupported", 31 },
    { "FractalCompressionNotSupported", 32 },
    { "ImageColumnOrRowSizeIsNotSupported", 33 },
    { "ImageDoesNotHaveAMatteChannel", 34 },
    { "ImageIsNotTiled", 35 },
    { "ImageTypeNotSupported", 36 },
    { "IncompatibleSizeOfDouble", 37 },
    { "IrregularChannelGeometryNotSupported", 38 },
    { "JNGCompressionNotSupported", 39 },
    { "JPEGCompressionNotSupported", 40 },
    { "JPEGEmbeddingFailed", 41 },
    { "LocationTypeIsNotSupported", 42 },
    { "MapStorageTypeIsNotSupported", 43 },
    { "MSBByteOrderNotSupported", 44 },
    { "MultidimensionalMatricesAreNotSupported", 45 },
    { "MultipleRecordListNotSupported", 46 },
    { "No8BIMDataIsAvailable", 47 },
    { "NoAPP1DataIsAvailable", 48 },
    { "NoBitmapOnClipboard", 49 },
    { "NoColorProfileAvailable", 50 },
    { "NoDataReturned", 51 },
    { "NoImageVectorGraphics", 52 },
    { "NoIPTCInfoWasFound", 53 },
    { "NoIPTCProfileAvailable", 54 },
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
    { "NotEnoughTiles", 109 },
    { "StaticPlanesValueNotEqualToOne", 110 },
    { "TooMuchImageDataInFile", 111 },
    { "UnableToReadColormapFromDumpFile", 112 },
    { "UnableToReadColorProfile", 113 },
    { "UnableToReadExtensionBlock", 114 },
    { "UnableToReadGenericProfile", 115 },
    { "UnableToReadImageData", 116 },
    { "UnableToReadImageHeader", 117 },
    { "UnableToReadIPTCProfile", 118 },
    { "UnableToReadPixmapFromDumpFile", 119 },
    { "UnableToReadSubImageData", 120 },
    { "UnableToReadVIDImage", 121 },
    { "UnableToReadWindowNameFromDumpFile", 122 },
    { "UnableToRunlengthDecodeImage", 123 },
    { "UnableToUncompressImage", 124 },
    { "UnexpectedEndOfFile", 125 },
    { "UnexpectedSamplingFactor", 126 },
    { "UnknownPatternType", 127 },
    { "UnrecognizedBitsPerPixel", 128 },
    { "UnrecognizedImageCompression", 129 },
    { "UnrecognizedXWDHeader", 130 },
    { "UnableToPersistKey", 131 },
    { "CompressionNotValid", 132 },
    { "ImproperImageHeader", 133 },
    { "NegativeOrZeroImageSize", 134 },
    { "NonOS2HeaderSizeError", 135 },
    { "SkipToSyncByte", 136 },
    { "StaticPlanesValueNotEqualToOne", 137 },
    { "UnrecognizedBitsPerPixel", 138 },
    { "UnrecognizedImageCompression", 139 },
    { "UnrecognizedNumberOfColors", 140 },
    { "DelegateFailed", 141 },
    { "FailedToComputeOutputSize", 142 },
    { "FailedToRenderFile", 143 },
    { "FailedToScanFile", 144 },
    { "NoTagFound", 145 },
    { "PostscriptDelegateFailed", 146 },
    { "UnableToCreateImage", 147 },
    { "UnableToCreateImageComponent", 148 },
    { "UnableToDecodeImageFile", 149 },
    { "UnableToEncodeImageFile", 150 },
    { "UnableToInitializeFPXLibrary", 151 },
    { "UnableToInitializeWMFLibrary", 152 },
    { "UnableToManageJP2Stream", 153 },
    { "UnableToWriteSVGFormat", 154 },
    { "Default", 155 },
    { "Default", 156 },
    { "AlreadyPushingPatternDefinition", 157 },
    { "NonconformingDrawingPrimitiveDefinition", 158 },
    { "UnableToPrint", 159 },
    { "UnbalancedGraphicContextPushPop", 160 },
    { "Default", 161 },
    { "NotARelativeURL", 162 },
    { "NotCurrentlyPushingPatternDefinition", 163 },
    { "URLNotFound", 164 },
    { "UnableToCreateTemporaryFile", 165 },
    { "UnableToOpenFile", 166 },
    { "UnableToWriteFile", 167 },
    { "Default", 168 },
    { "Default", 169 },
    { "AngleIsDiscontinuous", 170 },
    { "ColorspaceColorProfileMismatch", 171 },
    { "ImageColorspaceDiffers", 172 },
    { "ImageDoesNotContainResolution", 173 },
    { "ImageOpacityDiffers", 174 },
    { "ImageSequenceIsRequired", 175 },
    { "ImageSizeDiffers", 176 },
    { "InvalidColormapIndex", 177 },
    { "LeftAndRightImageSizesDiffer", 178 },
    { "NoImagesWereFound", 179 },
    { "NoImagesWereLoaded", 180 },
    { "NoLocaleImageAttribute", 181 },
    { "TooManyClusters", 182 },
    { "UnableToAppendImage", 183 },
    { "UnableToAssignProfile", 184 },
    { "UnableToAverageImage", 185 },
    { "UnableToCoalesceImage", 186 },
    { "UnableToCompareImages", 187 },
    { "UnableToCreateImageMosaic", 188 },
    { "UnableToCreateStereoImage", 189 },
    { "UnableToDeconstructImageSequence", 190 },
    { "UnableToFlattenImage", 191 },
    { "UnableToGetClipMask", 192 },
    { "UnableToResizeImage", 193 },
    { "UnableToSegmentImage", 194 },
    { "UnableToSetClipMask", 195 },
    { "UnableToShearImage", 196 },
    { "WidthOrHeightExceedsLimit", 197 },
    { "UnableToPersistKey", 198 },
    { "Default", 199 },
    { "DPSLibraryIsNotAvailable", 200 },
    { "FPXLibraryIsNotAvailable", 201 },
    { "FreeTypeLibraryIsNotAvailable", 202 },
    { "JPEGLibraryIsNotAvailable", 203 },
    { "LCMSLibraryIsNotAvailable", 204 },
    { "LZWEncodingNotEnabled", 205 },
    { "NoDecodeDelegateForThisImageFormat", 206 },
    { "NoEncodeDelegateForThisImageFormat", 207 },
    { "TIFFLibraryIsNotAvailable", 208 },
    { "XMLLibraryIsNotAvailable", 209 },
    { "XWindowLibraryIsNotAvailable", 210 },
    { "ZipLibraryIsNotAvailable", 211 },
    { "Default", 212 },
    { "Default", 213 },
    { "FailedToCloseModule", 214 },
    { "FailedToFindSymbol", 215 },
    { "UnableToLoadModule", 216 },
    { "UnableToRegisterImageFormat", 217 },
    { "UnrecognizedModule", 218 },
    { "UnableToInitializeModuleLoader", 219 },
    { "Default", 220 },
    { "BevelWidthIsNegative", 221 },
    { "ColorSeparatedImageRequired", 222 },
    { "FrameIsLessThanImageSize", 223 },
    { "GeometryDimensionsAreZero", 224 },
    { "GeometryDoesNotContainImage", 225 },
    { "ImagesAreNotTheSameSize", 226 },
    { "ImageSizeMustExceedBevelWidth", 227 },
    { "ImageSmallerThanKernelWidth", 228 },
    { "ImageSmallerThanRadius", 229 },
    { "ImageWidthsOrHeightsDiffer", 230 },
    { "InputImagesAlreadySpecified", 231 },
    { "KernelRadiusIsTooSmall", 232 },
    { "KernelWidthMustBeAnOddNumber", 233 },
    { "MissingAnImageFilename", 234 },
    { "MissingArgument", 235 },
    { "MustSpecifyAnImageName", 236 },
    { "MustSpecifyImageSize", 237 },
    { "NoBlobDefined", 238 },
    { "NoImagesDefined", 239 },
    { "NonzeroWidthAndHeightRequired", 240 },
    { "NoProfileNameWasGiven", 241 },
    { "NullBlobArgument", 242 },
    { "ReferenceImageRequired", 243 },
    { "ReferenceIsNotMyType", 244 },
    { "SteganoImageRequired", 245 },
    { "StereoImageRequired", 246 },
    { "SubimageSpecificationReturnsNoImages", 247 },
    { "UnableToAddOrRemoveProfile", 248 },
    { "UnableToAllocateICCProfile", 249 },
    { "UnableToAverageImageSequence", 250 },
    { "UnableToBlurImage", 251 },
    { "UnableToChopImage", 252 },
    { "UnableToConstituteImage", 253 },
    { "UnableToConvolveImage", 254 },
    { "UnableToEdgeImage", 255 },
    { "UnableToEqualizeImage", 256 },
    { "UnableToFilterImage", 257 },
    { "UnableToFormatImageMetadata", 258 },
    { "UnableToFrameImage", 259 },
    { "UnableToOilPaintImage", 260 },
    { "UnableToPaintImage", 261 },
    { "UnableToRaiseImage", 262 },
    { "UnableToSharpenImage", 263 },
    { "UnableToThresholdImage", 264 },
    { "UnableToWaveImage", 265 },
    { "UnrecognizedAttribute", 266 },
    { "UnrecognizedChannelType", 267 },
    { "UnrecognizedColor", 268 },
    { "UnrecognizedColormapType", 269 },
    { "UnrecognizedColorspace", 270 },
    { "UnrecognizedCommand", 271 },
    { "UnrecognizedComposeOperator", 272 },
    { "UnrecognizedDisposeMethod", 273 },
    { "UnrecognizedElement", 274 },
    { "UnrecognizedEndianType", 275 },
    { "UnrecognizedGravityType", 276 },
    { "UnrecognizedImageCompression", 277 },
    { "UnrecognizedImageFilter", 278 },
    { "UnrecognizedImageFormat", 279 },
    { "UnrecognizedImageMode", 280 },
    { "UnrecognizedImageType", 281 },
    { "UnrecognizedIntentType", 282 },
    { "UnrecognizedInterlaceType", 283 },
    { "UnrecognizedListType", 284 },
    { "UnrecognizedModeType", 285 },
    { "UnrecognizedNoiseType", 286 },
    { "UnrecognizedOperator", 287 },
    { "UnrecognizedOption", 288 },
    { "UnrecognizedPerlMagickMethod", 289 },
    { "UnrecognizedPixelMap", 290 },
    { "UnrecognizedPreviewType", 291 },
    { "UnrecognizedResourceType", 292 },
    { "UnrecognizedType", 293 },
    { "UnrecognizedUnitsType", 294 },
    { "UnrecognizedVirtualPixelMethod", 295 },
    { "UsageError", 296 },
    { "InvalidColorspaceType", 297 },
    { "InvalidEndianType", 298 },
    { "InvalidImageType", 299 },
    { "InvalidInterlaceType", 300 },
    { "MissingAnImageFilename", 301 },
    { "MissingArgument", 302 },
    { "NoImagesWereLoaded", 303 },
    { "OptionLengthExceedsLimit", 304 },
    { "UnableToOpenXServer", 305 },
    { "UnableToPersistKey", 306 },
    { "UnrecognizedColormapType", 307 },
    { "UnrecognizedColorspaceType", 308 },
    { "UnrecognizedDisposeMethod", 309 },
    { "UnrecognizedEndianType", 310 },
    { "UnrecognizedFilterType", 311 },
    { "UnrecognizedImageCompressionType", 312 },
    { "UnrecognizedImageType", 313 },
    { "UnrecognizedInterlaceType", 314 },
    { "UnrecognizedOption", 315 },
    { "UnrecognizedResourceType", 316 },
    { "UnrecognizedVirtualPixelMethod", 317 },
    { "UnrecognizedColor", 318 },
    { "ImageExpected", 319 },
    { "ImageInfoExpected", 320 },
    { "StructureSizeMismatch", 321 },
    { "UnableToGetRegistryID", 322 },
    { "UnableToLocateImage", 323 },
    { "UnableToSetRegistry", 324 },
    { "Default", 325 },
    { "Default", 326 },
    { "CacheResourcesExhausted", 327 },
    { "MemoryAllocationFailed", 328 },
    { "NoPixelsDefinedInCache", 329 },
    { "PixelCacheAllocationFailed", 330 },
    { "UnableToAddColorProfile", 331 },
    { "UnableToAddGenericProfile", 332 },
    { "UnableToAddIPTCProfile", 333 },
    { "UnableToAllocateCoefficients", 334 },
    { "UnableToAllocateColormap", 335 },
    { "UnableToAllocateString", 336 },
    { "UnableToAnnotateImage", 337 },
    { "UnableToAverageImageSequence", 338 },
    { "UnableToCloneImage", 339 },
    { "UnableToComputeImageSignature", 340 },
    { "UnableToConstituteImage", 341 },
    { "UnableToConvertFont", 342 },
    { "UnableToConvertStringToTokens", 343 },
    { "UnableToCreateColormap", 344 },
    { "UnableToCreateColorTransform", 345 },
    { "UnableToCreateCommandWidget", 346 },
    { "UnableToCreateImageGroup", 347 },
    { "UnableToCreateImageMontage", 348 },
    { "UnableToCreateXWindow", 349 },
    { "UnableToCropImage", 350 },
    { "UnableToDespeckleImage", 351 },
    { "UnableToDetermineImageClass", 352 },
    { "UnableToDetermineTheNumberOfImageColors", 353 },
    { "UnableToDitherImage", 354 },
    { "UnableToDrawOnImage", 355 },
    { "UnableToEdgeImage", 356 },
    { "UnableToEmbossImage", 357 },
    { "UnableToEnhanceImage", 358 },
    { "UnableToFloodfillImage", 359 },
    { "UnableToGammaCorrectImage", 360 },
    { "UnableToGetBestIconSize", 361 },
    { "UnableToGetFromRegistry", 362 },
    { "UnableToGetPackageInfo", 363 },
    { "UnableToLevelImage", 364 },
    { "UnableToMagnifyImage", 365 },
    { "UnableToManageColor", 366 },
    { "UnableToMapImage", 367 },
    { "UnableToMapImageSequence", 368 },
    { "UnableToMedianFilterImage", 369 },
    { "UnableToMotionBlurImage", 370 },
    { "UnableToNoiseFilterImage", 371 },
    { "UnableToNormalizeImage", 372 },
    { "UnableToOpenColorProfile", 373 },
    { "UnableToQuantizeImage", 374 },
    { "UnableToQuantizeImageSequence", 375 },
    { "UnableToReadTextChunk", 376 },
    { "UnableToReadXImage", 377 },
    { "UnableToReadXServerColormap", 378 },
    { "UnableToResizeImage", 379 },
    { "UnableToRotateImage", 380 },
    { "UnableToSampleImage", 381 },
    { "UnableToScaleImage", 382 },
    { "UnableToSelectImage", 383 },
    { "UnableToSharpenImage", 384 },
    { "UnableToShaveImage", 385 },
    { "UnableToShearImage", 386 },
    { "UnableToSortImageColormap", 387 },
    { "UnableToThresholdImage", 388 },
    { "UnableToTransformColorspace", 389 },
    { "MemoryAllocationFailed", 390 },
    { "UnableToAllocateAscii85Info", 391 },
    { "UnableToAllocateCacheInfo", 392 },
    { "UnableToAllocateCacheView", 393 },
    { "UnableToAllocateColorInfo", 394 },
    { "UnableToAllocateDashPattern", 395 },
    { "UnableToAllocateDelegateInfo", 396 },
    { "UnableToAllocateDerivatives", 397 },
    { "UnableToAllocateDrawContext", 398 },
    { "UnableToAllocateDrawInfo", 399 },
    { "UnableToAllocateDrawingWand", 400 },
    { "UnableToAllocateGammaMap", 401 },
    { "UnableToAllocateImage", 402 },
    { "UnableToAllocateImagePixels", 403 },
    { "UnableToAllocateLogInfo", 404 },
    { "UnableToAllocateMagicInfo", 405 },
    { "UnableToAllocateMagickInfo", 406 },
    { "UnableToAllocateModuleInfo", 407 },
    { "UnableToAllocateMontageInfo", 408 },
    { "UnableToAllocateQuantizeInfo", 409 },
    { "UnableToAllocateRegistryInfo", 410 },
    { "UnableToAllocateSemaphoreInfo", 411 },
    { "UnableToAllocateString", 412 },
    { "UnableToAllocateTypeInfo", 413 },
    { "UnableToAllocateWand", 414 },
    { "UnableToAnimateImageSequence", 415 },
    { "UnableToCloneBlobInfo", 416 },
    { "UnableToCloneCacheInfo", 417 },
    { "UnableToCloneImage", 418 },
    { "UnableToCloneImageInfo", 419 },
    { "UnableToConcatenateString", 420 },
    { "UnableToConvertText", 421 },
    { "UnableToCreateColormap", 422 },
    { "UnableToDisplayImage", 423 },
    { "UnableToEscapeString", 424 },
    { "UnableToInterpretMSLImage", 425 },
    { "MemoryAllocationFailed", 426 },
    { "ImageDoesNotContainTheStreamGeometry", 427 },
    { "NoStreamHandlerIsDefined", 428 },
    { "PixelCacheIsNotOpen", 429 },
    { "UnableToAcquirePixelStream", 430 },
    { "UnableToSetPixelStream", 431 },
    { "UnableToSyncPixelStream", 432 },
    { "Default", 433 },
    { "Default", 434 },
    { "FontSubstitutionRequired", 435 },
    { "UnableToGetTypeMetrics", 436 },
    { "UnableToInitializeFreetypeLibrary", 437 },
    { "UnableToReadFont", 438 },
    { "UnrecognizedFontEncoding", 439 },
    { "Default", 440 },
    { "Default", 441 },
    { "InvalidColormapIndex", 442 },
    { "WandAPINotImplemented", 443 },
    { "WandContainsNoImageIndexs", 444 },
    { "WandContainsNoImages", 445 },
    { "ColorIsNotKnownToServer", 446 },
    { "NoWindowWithSpecifiedIDExists", 447 },
    { "StandardColormapIsNotInitialized", 448 },
    { "UnableToConnectToRemoteDisplay", 449 },
    { "UnableToCreateBitmap", 450 },
    { "UnableToCreateColormap", 451 },
    { "UnableToCreatePixmap", 452 },
    { "UnableToCreateProperty", 453 },
    { "UnableToCreateStandardColormap", 454 },
    { "UnableToDisplayImageInfo", 455 },
    { "UnableToGetProperty", 456 },
    { "UnableToGetStandardColormap", 457 },
    { "UnableToGetVisual", 458 },
    { "UnableToGrabMouse", 459 },
    { "UnableToLoadFont", 460 },
    { "UnableToMatchVisualToStandardColormap", 461 },
    { "UnableToOpenXServer", 462 },
    { "UnableToReadXAttributes", 463 },
    { "UnableToReadXWindowImage", 464 },
    { "UnrecognizedColormapType", 465 },
    { "UnrecognizedGravityType", 466 },
    { "UnrecognizedVisualSpecifier", 467 },
    { "UnableToAllocateXHints", 468 },
    { "UnableToCreateCursor", 469 },
    { "UnableToCreateGraphicContext", 470 },
    { "UnableToCreateStandardColormap", 471 },
    { "UnableToCreateTextProperty", 472 },
    { "UnableToCreateXImage", 473 },
    { "UnableToCreateXPixmap", 474 },
    { "UnableToCreateXWindow", 475 },
    { "UnableToDisplayImage", 476 },
    { "UnableToDitherImage", 477 },
    { "UnableToGetPixelInfo", 478 },
    { "UnableToGetVisual", 479 },
    { "UnableToLoadFont", 480 },
    { "UnableToMakeXWindow", 481 },
    { "UnableToOpenXServer", 482 },
    { "UnableToViewFonts", 483 },
    { "UnableToGetVisual", 484 },
    { "UsingDefaultVisual", 485 },
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
    "Colormap size exceeds limit",
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
    "Wand API not implemented `%.1024s",
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
