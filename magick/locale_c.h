#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 482

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
#define MGK_ImageErrorUnableToResizeImage 191
#define MGK_ImageErrorUnableToSegmentImage 192
#define MGK_ImageErrorUnableToSetClipMask 193
#define MGK_ImageErrorUnableToShearImage 194
#define MGK_ImageErrorWidthOrHeightExceedsLimit 195
#define MGK_ImageFatalErrorUnableToPersistKey 196
#define MGK_ImageWarningDefault 197
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 198
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 199
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 200
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 201
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 202
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 203
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 204
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 205
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 206
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 207
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 208
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 209
#define MGK_MissingDelegateFatalErrorDefault 210
#define MGK_MissingDelegateWarningDefault 211
#define MGK_ModuleErrorFailedToCloseModule 212
#define MGK_ModuleErrorFailedToFindSymbol 213
#define MGK_ModuleErrorUnableToLoadModule 214
#define MGK_ModuleErrorUnableToRegisterImageFormat 215
#define MGK_ModuleErrorUnrecognizedModule 216
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 217
#define MGK_ModuleWarningDefault 218
#define MGK_OptionErrorBevelWidthIsNegative 219
#define MGK_OptionErrorColorSeparatedImageRequired 220
#define MGK_OptionErrorFrameIsLessThanImageSize 221
#define MGK_OptionErrorGeometryDimensionsAreZero 222
#define MGK_OptionErrorGeometryDoesNotContainImage 223
#define MGK_OptionErrorImagesAreNotTheSameSize 224
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 225
#define MGK_OptionErrorImageSmallerThanKernelWidth 226
#define MGK_OptionErrorImageSmallerThanRadius 227
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 228
#define MGK_OptionErrorInputImagesAlreadySpecified 229
#define MGK_OptionErrorKernelRadiusIsTooSmall 230
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 231
#define MGK_OptionErrorMissingAnImageFilename 232
#define MGK_OptionErrorMissingArgument 233
#define MGK_OptionErrorMustSpecifyAnImageName 234
#define MGK_OptionErrorMustSpecifyImageSize 235
#define MGK_OptionErrorNoBlobDefined 236
#define MGK_OptionErrorNoImagesDefined 237
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 238
#define MGK_OptionErrorNoProfileNameWasGiven 239
#define MGK_OptionErrorNullBlobArgument 240
#define MGK_OptionErrorReferenceImageRequired 241
#define MGK_OptionErrorReferenceIsNotMyType 242
#define MGK_OptionErrorSteganoImageRequired 243
#define MGK_OptionErrorStereoImageRequired 244
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 245
#define MGK_OptionErrorUnableToAddOrRemoveProfile 246
#define MGK_OptionErrorUnableToAllocateICCProfile 247
#define MGK_OptionErrorUnableToAverageImageSequence 248
#define MGK_OptionErrorUnableToBlurImage 249
#define MGK_OptionErrorUnableToChopImage 250
#define MGK_OptionErrorUnableToConstituteImage 251
#define MGK_OptionErrorUnableToConvolveImage 252
#define MGK_OptionErrorUnableToEdgeImage 253
#define MGK_OptionErrorUnableToEqualizeImage 254
#define MGK_OptionErrorUnableToFilterImage 255
#define MGK_OptionErrorUnableToFormatImageMetadata 256
#define MGK_OptionErrorUnableToFrameImage 257
#define MGK_OptionErrorUnableToOilPaintImage 258
#define MGK_OptionErrorUnableToPaintImage 259
#define MGK_OptionErrorUnableToRaiseImage 260
#define MGK_OptionErrorUnableToSharpenImage 261
#define MGK_OptionErrorUnableToThresholdImage 262
#define MGK_OptionErrorUnableToWaveImage 263
#define MGK_OptionErrorUnrecognizedAttribute 264
#define MGK_OptionErrorUnrecognizedChannelType 265
#define MGK_OptionErrorUnrecognizedColor 266
#define MGK_OptionErrorUnrecognizedColormapType 267
#define MGK_OptionErrorUnrecognizedColorspace 268
#define MGK_OptionErrorUnrecognizedCommand 269
#define MGK_OptionErrorUnrecognizedComposeOperator 270
#define MGK_OptionErrorUnrecognizedDisposeMethod 271
#define MGK_OptionErrorUnrecognizedElement 272
#define MGK_OptionErrorUnrecognizedEndianType 273
#define MGK_OptionErrorUnrecognizedGravityType 274
#define MGK_OptionErrorUnrecognizedImageCompression 275
#define MGK_OptionErrorUnrecognizedImageFilter 276
#define MGK_OptionErrorUnrecognizedImageFormat 277
#define MGK_OptionErrorUnrecognizedImageMode 278
#define MGK_OptionErrorUnrecognizedImageType 279
#define MGK_OptionErrorUnrecognizedIntentType 280
#define MGK_OptionErrorUnrecognizedInterlaceType 281
#define MGK_OptionErrorUnrecognizedListType 282
#define MGK_OptionErrorUnrecognizedModeType 283
#define MGK_OptionErrorUnrecognizedNoiseType 284
#define MGK_OptionErrorUnrecognizedOperator 285
#define MGK_OptionErrorUnrecognizedOption 286
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 287
#define MGK_OptionErrorUnrecognizedPixelMap 288
#define MGK_OptionErrorUnrecognizedPreviewType 289
#define MGK_OptionErrorUnrecognizedResourceType 290
#define MGK_OptionErrorUnrecognizedType 291
#define MGK_OptionErrorUnrecognizedUnitsType 292
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 293
#define MGK_OptionErrorUsageError 294
#define MGK_OptionFatalErrorInvalidColorspaceType 295
#define MGK_OptionFatalErrorInvalidEndianType 296
#define MGK_OptionFatalErrorInvalidImageType 297
#define MGK_OptionFatalErrorInvalidInterlaceType 298
#define MGK_OptionFatalErrorMissingAnImageFilename 299
#define MGK_OptionFatalErrorMissingArgument 300
#define MGK_OptionFatalErrorNoImagesWereLoaded 301
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 302
#define MGK_OptionFatalErrorUnableToOpenXServer 303
#define MGK_OptionFatalErrorUnableToPersistKey 304
#define MGK_OptionFatalErrorUnrecognizedColormapType 305
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 306
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 307
#define MGK_OptionFatalErrorUnrecognizedEndianType 308
#define MGK_OptionFatalErrorUnrecognizedFilterType 309
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 310
#define MGK_OptionFatalErrorUnrecognizedImageType 311
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 312
#define MGK_OptionFatalErrorUnrecognizedOption 313
#define MGK_OptionFatalErrorUnrecognizedResourceType 314
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 315
#define MGK_OptionWarningUnrecognizedColor 316
#define MGK_RegistryErrorImageExpected 317
#define MGK_RegistryErrorImageInfoExpected 318
#define MGK_RegistryErrorStructureSizeMismatch 319
#define MGK_RegistryErrorUnableToGetRegistryID 320
#define MGK_RegistryErrorUnableToLocateImage 321
#define MGK_RegistryErrorUnableToSetRegistry 322
#define MGK_RegistryFatalErrorDefault 323
#define MGK_RegistryWarningDefault 324
#define MGK_ResourceLimitErrorCacheResourcesExhausted 325
#define MGK_ResourceLimitErrorMemoryAllocationFailed 326
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 327
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 328
#define MGK_ResourceLimitErrorUnableToAddColorProfile 329
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 330
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 331
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 332
#define MGK_ResourceLimitErrorUnableToAllocateColormap 333
#define MGK_ResourceLimitErrorUnableToAllocateString 334
#define MGK_ResourceLimitErrorUnableToAnnotateImage 335
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 336
#define MGK_ResourceLimitErrorUnableToCloneImage 337
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 338
#define MGK_ResourceLimitErrorUnableToConstituteImage 339
#define MGK_ResourceLimitErrorUnableToConvertFont 340
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 341
#define MGK_ResourceLimitErrorUnableToCreateColormap 342
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 343
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 344
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 345
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 346
#define MGK_ResourceLimitErrorUnableToCreateXWindow 347
#define MGK_ResourceLimitErrorUnableToCropImage 348
#define MGK_ResourceLimitErrorUnableToDespeckleImage 349
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 350
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 351
#define MGK_ResourceLimitErrorUnableToDitherImage 352
#define MGK_ResourceLimitErrorUnableToDrawOnImage 353
#define MGK_ResourceLimitErrorUnableToEdgeImage 354
#define MGK_ResourceLimitErrorUnableToEmbossImage 355
#define MGK_ResourceLimitErrorUnableToEnhanceImage 356
#define MGK_ResourceLimitErrorUnableToFloodfillImage 357
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 358
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 359
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 360
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 361
#define MGK_ResourceLimitErrorUnableToLevelImage 362
#define MGK_ResourceLimitErrorUnableToMagnifyImage 363
#define MGK_ResourceLimitErrorUnableToManageColor 364
#define MGK_ResourceLimitErrorUnableToMapImage 365
#define MGK_ResourceLimitErrorUnableToMapImageSequence 366
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 367
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 368
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 369
#define MGK_ResourceLimitErrorUnableToNormalizeImage 370
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 371
#define MGK_ResourceLimitErrorUnableToQuantizeImage 372
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 373
#define MGK_ResourceLimitErrorUnableToReadTextChunk 374
#define MGK_ResourceLimitErrorUnableToReadXImage 375
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 376
#define MGK_ResourceLimitErrorUnableToResizeImage 377
#define MGK_ResourceLimitErrorUnableToRotateImage 378
#define MGK_ResourceLimitErrorUnableToSampleImage 379
#define MGK_ResourceLimitErrorUnableToScaleImage 380
#define MGK_ResourceLimitErrorUnableToSelectImage 381
#define MGK_ResourceLimitErrorUnableToSharpenImage 382
#define MGK_ResourceLimitErrorUnableToShaveImage 383
#define MGK_ResourceLimitErrorUnableToShearImage 384
#define MGK_ResourceLimitErrorUnableToSortImageColormap 385
#define MGK_ResourceLimitErrorUnableToThresholdImage 386
#define MGK_ResourceLimitErrorUnableToTransformColorspace 387
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 388
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 389
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 390
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 391
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 392
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 393
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 394
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 395
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 396
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 397
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 398
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 399
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 400
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 401
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 402
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 403
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 404
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 405
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 406
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 407
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 408
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 409
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 410
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 411
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 412
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 413
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 414
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 415
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 416
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 417
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 418
#define MGK_ResourceLimitFatalErrorUnableToConvertText 419
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 420
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 421
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 422
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 423
#define MGK_ResourceLimitWarningMemoryAllocationFailed 424
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 425
#define MGK_StreamErrorNoStreamHandlerIsDefined 426
#define MGK_StreamErrorPixelCacheIsNotOpen 427
#define MGK_StreamErrorUnableToAcquirePixelStream 428
#define MGK_StreamErrorUnableToSetPixelStream 429
#define MGK_StreamErrorUnableToSyncPixelStream 430
#define MGK_StreamFatalErrorDefault 431
#define MGK_StreamWarningDefault 432
#define MGK_TypeErrorFontSubstitutionRequired 433
#define MGK_TypeErrorUnableToGetTypeMetrics 434
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 435
#define MGK_TypeErrorUnableToReadFont 436
#define MGK_TypeErrorUnrecognizedFontEncoding 437
#define MGK_TypeFatalErrorDefault 438
#define MGK_TypeWarningDefault 439
#define MGK_WandErrorInvalidColormapIndex 440
#define MGK_WandErrorWandContainsNoImageIndexs 441
#define MGK_WandErrorWandContainsNoImages 442
#define MGK_XServerErrorColorIsNotKnownToServer 443
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 444
#define MGK_XServerErrorStandardColormapIsNotInitialized 445
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 446
#define MGK_XServerErrorUnableToCreateBitmap 447
#define MGK_XServerErrorUnableToCreateColormap 448
#define MGK_XServerErrorUnableToCreatePixmap 449
#define MGK_XServerErrorUnableToCreateProperty 450
#define MGK_XServerErrorUnableToCreateStandardColormap 451
#define MGK_XServerErrorUnableToDisplayImageInfo 452
#define MGK_XServerErrorUnableToGetProperty 453
#define MGK_XServerErrorUnableToGetStandardColormap 454
#define MGK_XServerErrorUnableToGetVisual 455
#define MGK_XServerErrorUnableToGrabMouse 456
#define MGK_XServerErrorUnableToLoadFont 457
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 458
#define MGK_XServerErrorUnableToOpenXServer 459
#define MGK_XServerErrorUnableToReadXAttributes 460
#define MGK_XServerErrorUnableToReadXWindowImage 461
#define MGK_XServerErrorUnrecognizedColormapType 462
#define MGK_XServerErrorUnrecognizedGravityType 463
#define MGK_XServerErrorUnrecognizedVisualSpecifier 464
#define MGK_XServerFatalErrorUnableToAllocateXHints 465
#define MGK_XServerFatalErrorUnableToCreateCursor 466
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 467
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 468
#define MGK_XServerFatalErrorUnableToCreateTextProperty 469
#define MGK_XServerFatalErrorUnableToCreateXImage 470
#define MGK_XServerFatalErrorUnableToCreateXPixmap 471
#define MGK_XServerFatalErrorUnableToCreateXWindow 472
#define MGK_XServerFatalErrorUnableToDisplayImage 473
#define MGK_XServerFatalErrorUnableToDitherImage 474
#define MGK_XServerFatalErrorUnableToGetPixelInfo 475
#define MGK_XServerFatalErrorUnableToGetVisual 476
#define MGK_XServerFatalErrorUnableToLoadFont 477
#define MGK_XServerFatalErrorUnableToMakeXWindow 478
#define MGK_XServerFatalErrorUnableToOpenXServer 479
#define MGK_XServerFatalErrorUnableToViewFonts 480
#define MGK_XServerWarningUnableToGetVisual 481
#define MGK_XServerWarningUsingDefaultVisual 482

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
    { "Image/FatalError", 195, ImageFatalError },
    { "Image/Warning", 196, ImageWarning },
    { "Missing/Delegate/Error", 197, MissingDelegateError },
    { "Missing/Delegate/FatalError", 209, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 210, MissingDelegateWarning },
    { "Module/Error", 211, ModuleError },
    { "Module/FatalError", 216, ModuleFatalError },
    { "Module/Warning", 217, ModuleWarning },
    { "Option/Error", 218, OptionError },
    { "Option/FatalError", 294, OptionFatalError },
    { "Option/Warning", 315, OptionWarning },
    { "Registry/Error", 316, RegistryError },
    { "Registry/FatalError", 322, RegistryFatalError },
    { "Registry/Warning", 323, RegistryWarning },
    { "Resource/Limit/Error", 324, ResourceLimitError },
    { "Resource/Limit/FatalError", 387, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 423, ResourceLimitWarning },
    { "Stream/Error", 424, StreamError },
    { "Stream/FatalError", 430, StreamFatalError },
    { "Stream/Warning", 431, StreamWarning },
    { "Type/Error", 432, TypeError },
    { "Type/FatalError", 437, TypeFatalError },
    { "Type/Warning", 438, TypeWarning },
    { "Wand/Error", 439, WandError },
    { "XServer/Error", 442, XServerError },
    { "XServer/FatalError", 464, XServerFatalError },
    { "XServer/Warning", 480, XServerWarning },
    { 0, 482, UndefinedException }
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
    { "UnableToResizeImage", 191 },
    { "UnableToSegmentImage", 192 },
    { "UnableToSetClipMask", 193 },
    { "UnableToShearImage", 194 },
    { "WidthOrHeightExceedsLimit", 195 },
    { "UnableToPersistKey", 196 },
    { "Default", 197 },
    { "DPSLibraryIsNotAvailable", 198 },
    { "FPXLibraryIsNotAvailable", 199 },
    { "FreeTypeLibraryIsNotAvailable", 200 },
    { "JPEGLibraryIsNotAvailable", 201 },
    { "LCMSLibraryIsNotAvailable", 202 },
    { "LZWEncodingNotEnabled", 203 },
    { "NoDecodeDelegateForThisImageFormat", 204 },
    { "NoEncodeDelegateForThisImageFormat", 205 },
    { "TIFFLibraryIsNotAvailable", 206 },
    { "XMLLibraryIsNotAvailable", 207 },
    { "XWindowLibraryIsNotAvailable", 208 },
    { "ZipLibraryIsNotAvailable", 209 },
    { "Default", 210 },
    { "Default", 211 },
    { "FailedToCloseModule", 212 },
    { "FailedToFindSymbol", 213 },
    { "UnableToLoadModule", 214 },
    { "UnableToRegisterImageFormat", 215 },
    { "UnrecognizedModule", 216 },
    { "UnableToInitializeModuleLoader", 217 },
    { "Default", 218 },
    { "BevelWidthIsNegative", 219 },
    { "ColorSeparatedImageRequired", 220 },
    { "FrameIsLessThanImageSize", 221 },
    { "GeometryDimensionsAreZero", 222 },
    { "GeometryDoesNotContainImage", 223 },
    { "ImagesAreNotTheSameSize", 224 },
    { "ImageSizeMustExceedBevelWidth", 225 },
    { "ImageSmallerThanKernelWidth", 226 },
    { "ImageSmallerThanRadius", 227 },
    { "ImageWidthsOrHeightsDiffer", 228 },
    { "InputImagesAlreadySpecified", 229 },
    { "KernelRadiusIsTooSmall", 230 },
    { "KernelWidthMustBeAnOddNumber", 231 },
    { "MissingAnImageFilename", 232 },
    { "MissingArgument", 233 },
    { "MustSpecifyAnImageName", 234 },
    { "MustSpecifyImageSize", 235 },
    { "NoBlobDefined", 236 },
    { "NoImagesDefined", 237 },
    { "NonzeroWidthAndHeightRequired", 238 },
    { "NoProfileNameWasGiven", 239 },
    { "NullBlobArgument", 240 },
    { "ReferenceImageRequired", 241 },
    { "ReferenceIsNotMyType", 242 },
    { "SteganoImageRequired", 243 },
    { "StereoImageRequired", 244 },
    { "SubimageSpecificationReturnsNoImages", 245 },
    { "UnableToAddOrRemoveProfile", 246 },
    { "UnableToAllocateICCProfile", 247 },
    { "UnableToAverageImageSequence", 248 },
    { "UnableToBlurImage", 249 },
    { "UnableToChopImage", 250 },
    { "UnableToConstituteImage", 251 },
    { "UnableToConvolveImage", 252 },
    { "UnableToEdgeImage", 253 },
    { "UnableToEqualizeImage", 254 },
    { "UnableToFilterImage", 255 },
    { "UnableToFormatImageMetadata", 256 },
    { "UnableToFrameImage", 257 },
    { "UnableToOilPaintImage", 258 },
    { "UnableToPaintImage", 259 },
    { "UnableToRaiseImage", 260 },
    { "UnableToSharpenImage", 261 },
    { "UnableToThresholdImage", 262 },
    { "UnableToWaveImage", 263 },
    { "UnrecognizedAttribute", 264 },
    { "UnrecognizedChannelType", 265 },
    { "UnrecognizedColor", 266 },
    { "UnrecognizedColormapType", 267 },
    { "UnrecognizedColorspace", 268 },
    { "UnrecognizedCommand", 269 },
    { "UnrecognizedComposeOperator", 270 },
    { "UnrecognizedDisposeMethod", 271 },
    { "UnrecognizedElement", 272 },
    { "UnrecognizedEndianType", 273 },
    { "UnrecognizedGravityType", 274 },
    { "UnrecognizedImageCompression", 275 },
    { "UnrecognizedImageFilter", 276 },
    { "UnrecognizedImageFormat", 277 },
    { "UnrecognizedImageMode", 278 },
    { "UnrecognizedImageType", 279 },
    { "UnrecognizedIntentType", 280 },
    { "UnrecognizedInterlaceType", 281 },
    { "UnrecognizedListType", 282 },
    { "UnrecognizedModeType", 283 },
    { "UnrecognizedNoiseType", 284 },
    { "UnrecognizedOperator", 285 },
    { "UnrecognizedOption", 286 },
    { "UnrecognizedPerlMagickMethod", 287 },
    { "UnrecognizedPixelMap", 288 },
    { "UnrecognizedPreviewType", 289 },
    { "UnrecognizedResourceType", 290 },
    { "UnrecognizedType", 291 },
    { "UnrecognizedUnitsType", 292 },
    { "UnrecognizedVirtualPixelMethod", 293 },
    { "UsageError", 294 },
    { "InvalidColorspaceType", 295 },
    { "InvalidEndianType", 296 },
    { "InvalidImageType", 297 },
    { "InvalidInterlaceType", 298 },
    { "MissingAnImageFilename", 299 },
    { "MissingArgument", 300 },
    { "NoImagesWereLoaded", 301 },
    { "OptionLengthExceedsLimit", 302 },
    { "UnableToOpenXServer", 303 },
    { "UnableToPersistKey", 304 },
    { "UnrecognizedColormapType", 305 },
    { "UnrecognizedColorspaceType", 306 },
    { "UnrecognizedDisposeMethod", 307 },
    { "UnrecognizedEndianType", 308 },
    { "UnrecognizedFilterType", 309 },
    { "UnrecognizedImageCompressionType", 310 },
    { "UnrecognizedImageType", 311 },
    { "UnrecognizedInterlaceType", 312 },
    { "UnrecognizedOption", 313 },
    { "UnrecognizedResourceType", 314 },
    { "UnrecognizedVirtualPixelMethod", 315 },
    { "UnrecognizedColor", 316 },
    { "ImageExpected", 317 },
    { "ImageInfoExpected", 318 },
    { "StructureSizeMismatch", 319 },
    { "UnableToGetRegistryID", 320 },
    { "UnableToLocateImage", 321 },
    { "UnableToSetRegistry", 322 },
    { "Default", 323 },
    { "Default", 324 },
    { "CacheResourcesExhausted", 325 },
    { "MemoryAllocationFailed", 326 },
    { "NoPixelsDefinedInCache", 327 },
    { "PixelCacheAllocationFailed", 328 },
    { "UnableToAddColorProfile", 329 },
    { "UnableToAddGenericProfile", 330 },
    { "UnableToAddIPTCProfile", 331 },
    { "UnableToAllocateCoefficients", 332 },
    { "UnableToAllocateColormap", 333 },
    { "UnableToAllocateString", 334 },
    { "UnableToAnnotateImage", 335 },
    { "UnableToAverageImageSequence", 336 },
    { "UnableToCloneImage", 337 },
    { "UnableToComputeImageSignature", 338 },
    { "UnableToConstituteImage", 339 },
    { "UnableToConvertFont", 340 },
    { "UnableToConvertStringToTokens", 341 },
    { "UnableToCreateColormap", 342 },
    { "UnableToCreateColorTransform", 343 },
    { "UnableToCreateCommandWidget", 344 },
    { "UnableToCreateImageGroup", 345 },
    { "UnableToCreateImageMontage", 346 },
    { "UnableToCreateXWindow", 347 },
    { "UnableToCropImage", 348 },
    { "UnableToDespeckleImage", 349 },
    { "UnableToDetermineImageClass", 350 },
    { "UnableToDetermineTheNumberOfImageColors", 351 },
    { "UnableToDitherImage", 352 },
    { "UnableToDrawOnImage", 353 },
    { "UnableToEdgeImage", 354 },
    { "UnableToEmbossImage", 355 },
    { "UnableToEnhanceImage", 356 },
    { "UnableToFloodfillImage", 357 },
    { "UnableToGammaCorrectImage", 358 },
    { "UnableToGetBestIconSize", 359 },
    { "UnableToGetFromRegistry", 360 },
    { "UnableToGetPackageInfo", 361 },
    { "UnableToLevelImage", 362 },
    { "UnableToMagnifyImage", 363 },
    { "UnableToManageColor", 364 },
    { "UnableToMapImage", 365 },
    { "UnableToMapImageSequence", 366 },
    { "UnableToMedianFilterImage", 367 },
    { "UnableToMotionBlurImage", 368 },
    { "UnableToNoiseFilterImage", 369 },
    { "UnableToNormalizeImage", 370 },
    { "UnableToOpenColorProfile", 371 },
    { "UnableToQuantizeImage", 372 },
    { "UnableToQuantizeImageSequence", 373 },
    { "UnableToReadTextChunk", 374 },
    { "UnableToReadXImage", 375 },
    { "UnableToReadXServerColormap", 376 },
    { "UnableToResizeImage", 377 },
    { "UnableToRotateImage", 378 },
    { "UnableToSampleImage", 379 },
    { "UnableToScaleImage", 380 },
    { "UnableToSelectImage", 381 },
    { "UnableToSharpenImage", 382 },
    { "UnableToShaveImage", 383 },
    { "UnableToShearImage", 384 },
    { "UnableToSortImageColormap", 385 },
    { "UnableToThresholdImage", 386 },
    { "UnableToTransformColorspace", 387 },
    { "MemoryAllocationFailed", 388 },
    { "UnableToAllocateAscii85Info", 389 },
    { "UnableToAllocateCacheInfo", 390 },
    { "UnableToAllocateCacheView", 391 },
    { "UnableToAllocateColorInfo", 392 },
    { "UnableToAllocateDashPattern", 393 },
    { "UnableToAllocateDelegateInfo", 394 },
    { "UnableToAllocateDerivatives", 395 },
    { "UnableToAllocateDrawContext", 396 },
    { "UnableToAllocateDrawInfo", 397 },
    { "UnableToAllocateDrawingWand", 398 },
    { "UnableToAllocateGammaMap", 399 },
    { "UnableToAllocateImage", 400 },
    { "UnableToAllocateImagePixels", 401 },
    { "UnableToAllocateLogInfo", 402 },
    { "UnableToAllocateMagicInfo", 403 },
    { "UnableToAllocateMagickInfo", 404 },
    { "UnableToAllocateModuleInfo", 405 },
    { "UnableToAllocateMontageInfo", 406 },
    { "UnableToAllocateQuantizeInfo", 407 },
    { "UnableToAllocateRegistryInfo", 408 },
    { "UnableToAllocateSemaphoreInfo", 409 },
    { "UnableToAllocateString", 410 },
    { "UnableToAllocateTypeInfo", 411 },
    { "UnableToAllocateWand", 412 },
    { "UnableToAnimateImageSequence", 413 },
    { "UnableToCloneBlobInfo", 414 },
    { "UnableToCloneCacheInfo", 415 },
    { "UnableToCloneImage", 416 },
    { "UnableToCloneImageInfo", 417 },
    { "UnableToConcatenateString", 418 },
    { "UnableToConvertText", 419 },
    { "UnableToCreateColormap", 420 },
    { "UnableToDisplayImage", 421 },
    { "UnableToEscapeString", 422 },
    { "UnableToInterpretMSLImage", 423 },
    { "MemoryAllocationFailed", 424 },
    { "ImageDoesNotContainTheStreamGeometry", 425 },
    { "NoStreamHandlerIsDefined", 426 },
    { "PixelCacheIsNotOpen", 427 },
    { "UnableToAcquirePixelStream", 428 },
    { "UnableToSetPixelStream", 429 },
    { "UnableToSyncPixelStream", 430 },
    { "Default", 431 },
    { "Default", 432 },
    { "FontSubstitutionRequired", 433 },
    { "UnableToGetTypeMetrics", 434 },
    { "UnableToInitializeFreetypeLibrary", 435 },
    { "UnableToReadFont", 436 },
    { "UnrecognizedFontEncoding", 437 },
    { "Default", 438 },
    { "Default", 439 },
    { "InvalidColormapIndex", 440 },
    { "WandContainsNoImageIndexs", 441 },
    { "WandContainsNoImages", 442 },
    { "ColorIsNotKnownToServer", 443 },
    { "NoWindowWithSpecifiedIDExists", 444 },
    { "StandardColormapIsNotInitialized", 445 },
    { "UnableToConnectToRemoteDisplay", 446 },
    { "UnableToCreateBitmap", 447 },
    { "UnableToCreateColormap", 448 },
    { "UnableToCreatePixmap", 449 },
    { "UnableToCreateProperty", 450 },
    { "UnableToCreateStandardColormap", 451 },
    { "UnableToDisplayImageInfo", 452 },
    { "UnableToGetProperty", 453 },
    { "UnableToGetStandardColormap", 454 },
    { "UnableToGetVisual", 455 },
    { "UnableToGrabMouse", 456 },
    { "UnableToLoadFont", 457 },
    { "UnableToMatchVisualToStandardColormap", 458 },
    { "UnableToOpenXServer", 459 },
    { "UnableToReadXAttributes", 460 },
    { "UnableToReadXWindowImage", 461 },
    { "UnrecognizedColormapType", 462 },
    { "UnrecognizedGravityType", 463 },
    { "UnrecognizedVisualSpecifier", 464 },
    { "UnableToAllocateXHints", 465 },
    { "UnableToCreateCursor", 466 },
    { "UnableToCreateGraphicContext", 467 },
    { "UnableToCreateStandardColormap", 468 },
    { "UnableToCreateTextProperty", 469 },
    { "UnableToCreateXImage", 470 },
    { "UnableToCreateXPixmap", 471 },
    { "UnableToCreateXWindow", 472 },
    { "UnableToDisplayImage", 473 },
    { "UnableToDitherImage", 474 },
    { "UnableToGetPixelInfo", 475 },
    { "UnableToGetVisual", 476 },
    { "UnableToLoadFont", 477 },
    { "UnableToMakeXWindow", 478 },
    { "UnableToOpenXServer", 479 },
    { "UnableToViewFonts", 480 },
    { "UnableToGetVisual", 481 },
    { "UsingDefaultVisual", 482 },
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
