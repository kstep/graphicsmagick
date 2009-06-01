#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 498

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
#define MGK_CacheErrorUnableToAllocateCacheView 12
#define MGK_CacheErrorUnableToCloneCache 13
#define MGK_CacheErrorUnableToExtendCache 14
#define MGK_CacheErrorUnableToGetCacheNexus 15
#define MGK_CacheErrorUnableToGetPixelsFromCache 16
#define MGK_CacheErrorUnableToOpenCache 17
#define MGK_CacheErrorUnableToPeristPixelCache 18
#define MGK_CacheErrorUnableToReadPixelCache 19
#define MGK_CacheErrorUnableToSyncCache 20
#define MGK_CacheFatalErrorDiskAllocationFailed 21
#define MGK_CacheFatalErrorUnableToExtendPixelCache 22
#define MGK_CacheWarningDefault 23
#define MGK_CoderErrorColormapTooLarge 24
#define MGK_CoderErrorColormapTypeNotSupported 25
#define MGK_CoderErrorColorspaceModelIsNotSupported 26
#define MGK_CoderErrorColorTypeNotSupported 27
#define MGK_CoderErrorCompressionNotValid 28
#define MGK_CoderErrorDataEncodingSchemeIsNotSupported 29
#define MGK_CoderErrorDataStorageTypeIsNotSupported 30
#define MGK_CoderErrorDeltaPNGNotSupported 31
#define MGK_CoderErrorEncryptedWPGImageFileNotSupported 32
#define MGK_CoderErrorFractalCompressionNotSupported 33
#define MGK_CoderErrorImageColumnOrRowSizeIsNotSupported 34
#define MGK_CoderErrorImageDoesNotHaveAMatteChannel 35
#define MGK_CoderErrorImageIsNotTiled 36
#define MGK_CoderErrorImageTypeNotSupported 37
#define MGK_CoderErrorIncompatibleSizeOfDouble 38
#define MGK_CoderErrorIrregularChannelGeometryNotSupported 39
#define MGK_CoderErrorJNGCompressionNotSupported 40
#define MGK_CoderErrorJPEGCompressionNotSupported 41
#define MGK_CoderErrorJPEGEmbeddingFailed 42
#define MGK_CoderErrorLocationTypeIsNotSupported 43
#define MGK_CoderErrorMapStorageTypeIsNotSupported 44
#define MGK_CoderErrorMSBByteOrderNotSupported 45
#define MGK_CoderErrorMultidimensionalMatricesAreNotSupported 46
#define MGK_CoderErrorMultipleRecordListNotSupported 47
#define MGK_CoderErrorNo8BIMDataIsAvailable 48
#define MGK_CoderErrorNoAPP1DataIsAvailable 49
#define MGK_CoderErrorNoBitmapOnClipboard 50
#define MGK_CoderErrorNoColorProfileAvailable 51
#define MGK_CoderErrorNoDataReturned 52
#define MGK_CoderErrorNoImageVectorGraphics 53
#define MGK_CoderErrorNoIPTCInfoWasFound 54
#define MGK_CoderErrorNoIPTCProfileAvailable 55
#define MGK_CoderErrorNumberOfImagesIsNotSupported 56
#define MGK_CoderErrorOnlyContinuousTonePictureSupported 57
#define MGK_CoderErrorOnlyLevelZerofilesSupported 58
#define MGK_CoderErrorPNGCompressionNotSupported 59
#define MGK_CoderErrorPNGLibraryTooOld 60
#define MGK_CoderErrorRLECompressionNotSupported 61
#define MGK_CoderErrorSubsamplingRequiresEvenWidth 62
#define MGK_CoderErrorUnableToCopyProfile 63
#define MGK_CoderErrorUnableToCreateADC 64
#define MGK_CoderErrorUnableToCreateBitmap 65
#define MGK_CoderErrorUnableToDecompressImage 66
#define MGK_CoderErrorUnableToInitializeFPXLibrary 67
#define MGK_CoderErrorUnableToOpenBlob 68
#define MGK_CoderErrorUnableToReadAspectRatio 69
#define MGK_CoderErrorUnableToReadCIELABImages 70
#define MGK_CoderErrorUnableToReadSummaryInfo 71
#define MGK_CoderErrorUnableToSetAffineMatrix 72
#define MGK_CoderErrorUnableToSetAspectRatio 73
#define MGK_CoderErrorUnableToSetColorTwist 74
#define MGK_CoderErrorUnableToSetContrast 75
#define MGK_CoderErrorUnableToSetFilteringValue 76
#define MGK_CoderErrorUnableToSetImageComments 77
#define MGK_CoderErrorUnableToSetImageTitle 78
#define MGK_CoderErrorUnableToSetJPEGLevel 79
#define MGK_CoderErrorUnableToSetRegionOfInterest 80
#define MGK_CoderErrorUnableToSetSummaryInfo 81
#define MGK_CoderErrorUnableToTranslateText 82
#define MGK_CoderErrorUnableToWriteMPEGParameters 83
#define MGK_CoderErrorUnableToZipCompressImage 84
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 85
#define MGK_CoderErrorZipCompressionNotSupported 86
#define MGK_CoderFatalErrorDefault 87
#define MGK_CoderWarningLosslessToLossyJPEGConversion 88
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 89
#define MGK_ConfigureErrorRegistryKeyLookupFailed 90
#define MGK_ConfigureErrorUnableToAccessConfigureFile 91
#define MGK_ConfigureErrorUnableToAccessFontFile 92
#define MGK_ConfigureErrorUnableToAccessLogFile 93
#define MGK_ConfigureErrorUnableToAccessModuleFile 94
#define MGK_ConfigureFatalErrorDefault 95
#define MGK_ConfigureWarningDefault 96
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 97
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 98
#define MGK_CorruptImageErrorColormapExceeds256Colors 99
#define MGK_CorruptImageErrorCorruptImage 100
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 101
#define MGK_CorruptImageErrorImageTypeNotSupported 102
#define MGK_CorruptImageErrorImproperImageHeader 103
#define MGK_CorruptImageErrorInsufficientImageDataInFile 104
#define MGK_CorruptImageErrorInvalidColormapIndex 105
#define MGK_CorruptImageErrorInvalidFileFormatVersion 106
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 107
#define MGK_CorruptImageErrorMissingImageChannel 108
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 109
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 110
#define MGK_CorruptImageErrorNotEnoughTiles 111
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 112
#define MGK_CorruptImageErrorSubsamplingRequiresEvenWidth 113
#define MGK_CorruptImageErrorTooMuchImageDataInFile 114
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 115
#define MGK_CorruptImageErrorUnableToReadColorProfile 116
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 117
#define MGK_CorruptImageErrorUnableToReadGenericProfile 118
#define MGK_CorruptImageErrorUnableToReadImageData 119
#define MGK_CorruptImageErrorUnableToReadImageHeader 120
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 121
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 122
#define MGK_CorruptImageErrorUnableToReadSubImageData 123
#define MGK_CorruptImageErrorUnableToReadVIDImage 124
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 125
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 126
#define MGK_CorruptImageErrorUnableToUncompressImage 127
#define MGK_CorruptImageErrorUnexpectedEndOfFile 128
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 129
#define MGK_CorruptImageErrorUnknownPatternType 130
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 131
#define MGK_CorruptImageErrorUnrecognizedImageCompression 132
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 133
#define MGK_CorruptImageFatalErrorUnableToPersistKey 134
#define MGK_CorruptImageWarningCompressionNotValid 135
#define MGK_CorruptImageWarningImproperImageHeader 136
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 137
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 138
#define MGK_CorruptImageWarningSkipToSyncByte 139
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 140
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 141
#define MGK_CorruptImageWarningUnrecognizedImageCompression 142
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 143
#define MGK_DelegateErrorDelegateFailed 144
#define MGK_DelegateErrorFailedToComputeOutputSize 145
#define MGK_DelegateErrorFailedToRenderFile 146
#define MGK_DelegateErrorFailedToScanFile 147
#define MGK_DelegateErrorNoTagFound 148
#define MGK_DelegateErrorPostscriptDelegateFailed 149
#define MGK_DelegateErrorUnableToCreateImage 150
#define MGK_DelegateErrorUnableToCreateImageComponent 151
#define MGK_DelegateErrorUnableToDecodeImageFile 152
#define MGK_DelegateErrorUnableToEncodeImageFile 153
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 154
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 155
#define MGK_DelegateErrorUnableToManageJP2Stream 156
#define MGK_DelegateErrorUnableToWriteSVGFormat 157
#define MGK_DelegateFatalErrorDefault 158
#define MGK_DelegateWarningDefault 159
#define MGK_DrawErrorAlreadyPushingPatternDefinition 160
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 161
#define MGK_DrawErrorUnableToPrint 162
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 163
#define MGK_DrawFatalErrorDefault 164
#define MGK_DrawWarningNotARelativeURL 165
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 166
#define MGK_DrawWarningURLNotFound 167
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 168
#define MGK_FileOpenErrorUnableToOpenFile 169
#define MGK_FileOpenErrorUnableToWriteFile 170
#define MGK_FileOpenFatalErrorDefault 171
#define MGK_FileOpenWarningDefault 172
#define MGK_ImageErrorAngleIsDiscontinuous 173
#define MGK_ImageErrorColorspaceColorProfileMismatch 174
#define MGK_ImageErrorImageColorspaceDiffers 175
#define MGK_ImageErrorImageDifferenceExceedsLimit 176
#define MGK_ImageErrorImageDoesNotContainResolution 177
#define MGK_ImageErrorImageOpacityDiffers 178
#define MGK_ImageErrorImageSequenceIsRequired 179
#define MGK_ImageErrorImageSizeDiffers 180
#define MGK_ImageErrorInvalidColormapIndex 181
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 182
#define MGK_ImageErrorNoImagesWereFound 183
#define MGK_ImageErrorNoImagesWereLoaded 184
#define MGK_ImageErrorNoLocaleImageAttribute 185
#define MGK_ImageErrorTooManyClusters 186
#define MGK_ImageErrorUnableToAppendImage 187
#define MGK_ImageErrorUnableToAssignProfile 188
#define MGK_ImageErrorUnableToAverageImage 189
#define MGK_ImageErrorUnableToCoalesceImage 190
#define MGK_ImageErrorUnableToCompareImages 191
#define MGK_ImageErrorUnableToCreateImageMosaic 192
#define MGK_ImageErrorUnableToCreateStereoImage 193
#define MGK_ImageErrorUnableToDeconstructImageSequence 194
#define MGK_ImageErrorUnableToFlattenImage 195
#define MGK_ImageErrorUnableToGetClipMask 196
#define MGK_ImageErrorUnableToResizeImage 197
#define MGK_ImageErrorUnableToSegmentImage 198
#define MGK_ImageErrorUnableToSetClipMask 199
#define MGK_ImageErrorUnableToShearImage 200
#define MGK_ImageErrorWidthOrHeightExceedsLimit 201
#define MGK_ImageFatalErrorUnableToPersistKey 202
#define MGK_ImageWarningDefault 203
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 204
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 205
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 206
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 207
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 208
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 209
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 210
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 211
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 212
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 213
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 214
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 215
#define MGK_MissingDelegateFatalErrorDefault 216
#define MGK_MissingDelegateWarningDefault 217
#define MGK_ModuleErrorFailedToCloseModule 218
#define MGK_ModuleErrorFailedToFindSymbol 219
#define MGK_ModuleErrorUnableToLoadModule 220
#define MGK_ModuleErrorUnableToRegisterImageFormat 221
#define MGK_ModuleErrorUnrecognizedModule 222
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 223
#define MGK_ModuleWarningDefault 224
#define MGK_OptionErrorBevelWidthIsNegative 225
#define MGK_OptionErrorColorSeparatedImageRequired 226
#define MGK_OptionErrorFrameIsLessThanImageSize 227
#define MGK_OptionErrorGeometryDimensionsAreZero 228
#define MGK_OptionErrorGeometryDoesNotContainImage 229
#define MGK_OptionErrorImagesAreNotTheSameSize 230
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 231
#define MGK_OptionErrorImageSmallerThanKernelWidth 232
#define MGK_OptionErrorImageSmallerThanRadius 233
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 234
#define MGK_OptionErrorInputImagesAlreadySpecified 235
#define MGK_OptionErrorKernelRadiusIsTooSmall 236
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 237
#define MGK_OptionErrorMatrixOrderOutOfRange 238
#define MGK_OptionErrorMissingAnImageFilename 239
#define MGK_OptionErrorMissingArgument 240
#define MGK_OptionErrorMustSpecifyAnImageName 241
#define MGK_OptionErrorMustSpecifyImageSize 242
#define MGK_OptionErrorNoBlobDefined 243
#define MGK_OptionErrorNoImagesDefined 244
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 245
#define MGK_OptionErrorNoProfileNameWasGiven 246
#define MGK_OptionErrorNullBlobArgument 247
#define MGK_OptionErrorReferenceImageRequired 248
#define MGK_OptionErrorReferenceIsNotMyType 249
#define MGK_OptionErrorRequestDidNotReturnAnImage 250
#define MGK_OptionErrorSteganoImageRequired 251
#define MGK_OptionErrorStereoImageRequired 252
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 253
#define MGK_OptionErrorUnableToAddOrRemoveProfile 254
#define MGK_OptionErrorUnableToAverageImageSequence 255
#define MGK_OptionErrorUnableToBlurImage 256
#define MGK_OptionErrorUnableToChopImage 257
#define MGK_OptionErrorUnableToColorMatrixImage 258
#define MGK_OptionErrorUnableToConstituteImage 259
#define MGK_OptionErrorUnableToConvolveImage 260
#define MGK_OptionErrorUnableToEdgeImage 261
#define MGK_OptionErrorUnableToEqualizeImage 262
#define MGK_OptionErrorUnableToFilterImage 263
#define MGK_OptionErrorUnableToFormatImageMetadata 264
#define MGK_OptionErrorUnableToFrameImage 265
#define MGK_OptionErrorUnableToOilPaintImage 266
#define MGK_OptionErrorUnableToPaintImage 267
#define MGK_OptionErrorUnableToRaiseImage 268
#define MGK_OptionErrorUnableToSharpenImage 269
#define MGK_OptionErrorUnableToThresholdImage 270
#define MGK_OptionErrorUnableToWaveImage 271
#define MGK_OptionErrorUnrecognizedAttribute 272
#define MGK_OptionErrorUnrecognizedChannelType 273
#define MGK_OptionErrorUnrecognizedColor 274
#define MGK_OptionErrorUnrecognizedColormapType 275
#define MGK_OptionErrorUnrecognizedColorspace 276
#define MGK_OptionErrorUnrecognizedCommand 277
#define MGK_OptionErrorUnrecognizedComposeOperator 278
#define MGK_OptionErrorUnrecognizedDisposeMethod 279
#define MGK_OptionErrorUnrecognizedElement 280
#define MGK_OptionErrorUnrecognizedEndianType 281
#define MGK_OptionErrorUnrecognizedGravityType 282
#define MGK_OptionErrorUnrecognizedHighlightStyle 283
#define MGK_OptionErrorUnrecognizedImageCompression 284
#define MGK_OptionErrorUnrecognizedImageFilter 285
#define MGK_OptionErrorUnrecognizedImageFormat 286
#define MGK_OptionErrorUnrecognizedImageMode 287
#define MGK_OptionErrorUnrecognizedImageType 288
#define MGK_OptionErrorUnrecognizedIntentType 289
#define MGK_OptionErrorUnrecognizedInterlaceType 290
#define MGK_OptionErrorUnrecognizedListType 291
#define MGK_OptionErrorUnrecognizedMetric 292
#define MGK_OptionErrorUnrecognizedModeType 293
#define MGK_OptionErrorUnrecognizedNoiseType 294
#define MGK_OptionErrorUnrecognizedOperator 295
#define MGK_OptionErrorUnrecognizedOption 296
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 297
#define MGK_OptionErrorUnrecognizedPixelMap 298
#define MGK_OptionErrorUnrecognizedPreviewType 299
#define MGK_OptionErrorUnrecognizedResourceType 300
#define MGK_OptionErrorUnrecognizedType 301
#define MGK_OptionErrorUnrecognizedUnitsType 302
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 303
#define MGK_OptionErrorUnsupportedSamplingFactor 304
#define MGK_OptionErrorUsageError 305
#define MGK_OptionFatalErrorInvalidColorspaceType 306
#define MGK_OptionFatalErrorInvalidEndianType 307
#define MGK_OptionFatalErrorInvalidImageType 308
#define MGK_OptionFatalErrorInvalidInterlaceType 309
#define MGK_OptionFatalErrorMissingAnImageFilename 310
#define MGK_OptionFatalErrorMissingArgument 311
#define MGK_OptionFatalErrorNoImagesWereLoaded 312
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 313
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 314
#define MGK_OptionFatalErrorUnableToOpenXServer 315
#define MGK_OptionFatalErrorUnableToPersistKey 316
#define MGK_OptionFatalErrorUnrecognizedColormapType 317
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 318
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 319
#define MGK_OptionFatalErrorUnrecognizedEndianType 320
#define MGK_OptionFatalErrorUnrecognizedFilterType 321
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 322
#define MGK_OptionFatalErrorUnrecognizedImageType 323
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 324
#define MGK_OptionFatalErrorUnrecognizedOption 325
#define MGK_OptionFatalErrorUnrecognizedResourceType 326
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 327
#define MGK_OptionWarningUnrecognizedColor 328
#define MGK_RegistryErrorImageExpected 329
#define MGK_RegistryErrorImageInfoExpected 330
#define MGK_RegistryErrorStructureSizeMismatch 331
#define MGK_RegistryErrorUnableToGetRegistryID 332
#define MGK_RegistryErrorUnableToLocateImage 333
#define MGK_RegistryErrorUnableToSetRegistry 334
#define MGK_RegistryFatalErrorDefault 335
#define MGK_RegistryWarningDefault 336
#define MGK_ResourceLimitErrorCacheResourcesExhausted 337
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 338
#define MGK_ResourceLimitErrorMemoryAllocationFailed 339
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 340
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 341
#define MGK_ResourceLimitErrorUnableToAddColorProfile 342
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 343
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 344
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 345
#define MGK_ResourceLimitErrorUnableToAllocateColormap 346
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 347
#define MGK_ResourceLimitErrorUnableToAllocateString 348
#define MGK_ResourceLimitErrorUnableToAnnotateImage 349
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 350
#define MGK_ResourceLimitErrorUnableToCloneImage 351
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 352
#define MGK_ResourceLimitErrorUnableToConstituteImage 353
#define MGK_ResourceLimitErrorUnableToConvertFont 354
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 355
#define MGK_ResourceLimitErrorUnableToCreateColormap 356
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 357
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 358
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 359
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 360
#define MGK_ResourceLimitErrorUnableToCreateXWindow 361
#define MGK_ResourceLimitErrorUnableToCropImage 362
#define MGK_ResourceLimitErrorUnableToDespeckleImage 363
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 364
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 365
#define MGK_ResourceLimitErrorUnableToDitherImage 366
#define MGK_ResourceLimitErrorUnableToDrawOnImage 367
#define MGK_ResourceLimitErrorUnableToEdgeImage 368
#define MGK_ResourceLimitErrorUnableToEmbossImage 369
#define MGK_ResourceLimitErrorUnableToEnhanceImage 370
#define MGK_ResourceLimitErrorUnableToFloodfillImage 371
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 372
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 373
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 374
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 375
#define MGK_ResourceLimitErrorUnableToLevelImage 376
#define MGK_ResourceLimitErrorUnableToMagnifyImage 377
#define MGK_ResourceLimitErrorUnableToManageColor 378
#define MGK_ResourceLimitErrorUnableToMapImage 379
#define MGK_ResourceLimitErrorUnableToMapImageSequence 380
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 381
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 382
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 383
#define MGK_ResourceLimitErrorUnableToNormalizeImage 384
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 385
#define MGK_ResourceLimitErrorUnableToQuantizeImage 386
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 387
#define MGK_ResourceLimitErrorUnableToReadTextChunk 388
#define MGK_ResourceLimitErrorUnableToReadXImage 389
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 390
#define MGK_ResourceLimitErrorUnableToResizeImage 391
#define MGK_ResourceLimitErrorUnableToRotateImage 392
#define MGK_ResourceLimitErrorUnableToSampleImage 393
#define MGK_ResourceLimitErrorUnableToScaleImage 394
#define MGK_ResourceLimitErrorUnableToSelectImage 395
#define MGK_ResourceLimitErrorUnableToSharpenImage 396
#define MGK_ResourceLimitErrorUnableToShaveImage 397
#define MGK_ResourceLimitErrorUnableToShearImage 398
#define MGK_ResourceLimitErrorUnableToSortImageColormap 399
#define MGK_ResourceLimitErrorUnableToThresholdImage 400
#define MGK_ResourceLimitErrorUnableToTransformColorspace 401
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 402
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 403
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 404
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 405
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 406
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 407
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 408
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 409
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 410
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 411
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 412
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 413
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 414
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 415
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 416
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 417
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 418
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 419
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 420
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 421
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 422
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 423
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 424
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 425
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 426
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 427
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 428
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 429
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 430
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 431
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 432
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 433
#define MGK_ResourceLimitFatalErrorUnableToConvertText 434
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 435
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 436
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 437
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 438
#define MGK_ResourceLimitWarningMemoryAllocationFailed 439
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 440
#define MGK_StreamErrorNoStreamHandlerIsDefined 441
#define MGK_StreamErrorPixelCacheIsNotOpen 442
#define MGK_StreamErrorUnableToAcquirePixelStream 443
#define MGK_StreamErrorUnableToSetPixelStream 444
#define MGK_StreamErrorUnableToSyncPixelStream 445
#define MGK_StreamFatalErrorDefault 446
#define MGK_StreamWarningDefault 447
#define MGK_TypeErrorFontSubstitutionRequired 448
#define MGK_TypeErrorUnableToGetTypeMetrics 449
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 450
#define MGK_TypeErrorUnableToReadFont 451
#define MGK_TypeErrorUnrecognizedFontEncoding 452
#define MGK_TypeFatalErrorDefault 453
#define MGK_TypeWarningDefault 454
#define MGK_WandErrorInvalidColormapIndex 455
#define MGK_WandErrorWandAPINotImplemented 456
#define MGK_WandErrorWandContainsNoImageIndexs 457
#define MGK_WandErrorWandContainsNoImages 458
#define MGK_XServerErrorColorIsNotKnownToServer 459
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 460
#define MGK_XServerErrorStandardColormapIsNotInitialized 461
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 462
#define MGK_XServerErrorUnableToCreateBitmap 463
#define MGK_XServerErrorUnableToCreateColormap 464
#define MGK_XServerErrorUnableToCreatePixmap 465
#define MGK_XServerErrorUnableToCreateProperty 466
#define MGK_XServerErrorUnableToCreateStandardColormap 467
#define MGK_XServerErrorUnableToDisplayImageInfo 468
#define MGK_XServerErrorUnableToGetProperty 469
#define MGK_XServerErrorUnableToGetStandardColormap 470
#define MGK_XServerErrorUnableToGetVisual 471
#define MGK_XServerErrorUnableToGrabMouse 472
#define MGK_XServerErrorUnableToLoadFont 473
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 474
#define MGK_XServerErrorUnableToOpenXServer 475
#define MGK_XServerErrorUnableToReadXAttributes 476
#define MGK_XServerErrorUnableToReadXWindowImage 477
#define MGK_XServerErrorUnrecognizedColormapType 478
#define MGK_XServerErrorUnrecognizedGravityType 479
#define MGK_XServerErrorUnrecognizedVisualSpecifier 480
#define MGK_XServerFatalErrorUnableToAllocateXHints 481
#define MGK_XServerFatalErrorUnableToCreateCursor 482
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 483
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 484
#define MGK_XServerFatalErrorUnableToCreateTextProperty 485
#define MGK_XServerFatalErrorUnableToCreateXImage 486
#define MGK_XServerFatalErrorUnableToCreateXPixmap 487
#define MGK_XServerFatalErrorUnableToCreateXWindow 488
#define MGK_XServerFatalErrorUnableToDisplayImage 489
#define MGK_XServerFatalErrorUnableToDitherImage 490
#define MGK_XServerFatalErrorUnableToGetPixelInfo 491
#define MGK_XServerFatalErrorUnableToGetVisual 492
#define MGK_XServerFatalErrorUnableToLoadFont 493
#define MGK_XServerFatalErrorUnableToMakeXWindow 494
#define MGK_XServerFatalErrorUnableToOpenXServer 495
#define MGK_XServerFatalErrorUnableToViewFonts 496
#define MGK_XServerWarningUnableToGetVisual 497
#define MGK_XServerWarningUsingDefaultVisual 498

#endif

#if defined(_INCLUDE_CATEGORYMAP_TABLE_)
typedef struct _CategoryInfo{
  const char *name;
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
  const char *name;
  int offset;
  ExceptionType severityid;
} SeverityInfo;

static const SeverityInfo severity_map[] =
  {
    { "Blob/Error", 0, BlobError },
    { "Blob/FatalError", 7, BlobFatalError },
    { "Blob/Warning", 8, BlobWarning },
    { "Cache/Error", 9, CacheError },
    { "Cache/FatalError", 20, CacheFatalError },
    { "Cache/Warning", 22, CacheWarning },
    { "Coder/Error", 23, CoderError },
    { "Coder/FatalError", 86, CoderFatalError },
    { "Coder/Warning", 87, CoderWarning },
    { "Configure/Error", 88, ConfigureError },
    { "Configure/FatalError", 94, ConfigureFatalError },
    { "Configure/Warning", 95, ConfigureWarning },
    { "Corrupt/Image/Error", 96, CorruptImageError },
    { "Corrupt/Image/FatalError", 133, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 134, CorruptImageWarning },
    { "Delegate/Error", 143, DelegateError },
    { "Delegate/FatalError", 157, DelegateFatalError },
    { "Delegate/Warning", 158, DelegateWarning },
    { "Draw/Error", 159, DrawError },
    { "Draw/FatalError", 163, DrawFatalError },
    { "Draw/Warning", 164, DrawWarning },
    { "File/Open/Error", 167, FileOpenError },
    { "File/Open/FatalError", 170, FileOpenFatalError },
    { "File/Open/Warning", 171, FileOpenWarning },
    { "Image/Error", 172, ImageError },
    { "Image/FatalError", 201, ImageFatalError },
    { "Image/Warning", 202, ImageWarning },
    { "Missing/Delegate/Error", 203, MissingDelegateError },
    { "Missing/Delegate/FatalError", 215, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 216, MissingDelegateWarning },
    { "Module/Error", 217, ModuleError },
    { "Module/FatalError", 222, ModuleFatalError },
    { "Module/Warning", 223, ModuleWarning },
    { "Option/Error", 224, OptionError },
    { "Option/FatalError", 305, OptionFatalError },
    { "Option/Warning", 327, OptionWarning },
    { "Registry/Error", 328, RegistryError },
    { "Registry/FatalError", 334, RegistryFatalError },
    { "Registry/Warning", 335, RegistryWarning },
    { "Resource/Limit/Error", 336, ResourceLimitError },
    { "Resource/Limit/FatalError", 401, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 438, ResourceLimitWarning },
    { "Stream/Error", 439, StreamError },
    { "Stream/FatalError", 445, StreamFatalError },
    { "Stream/Warning", 446, StreamWarning },
    { "Type/Error", 447, TypeError },
    { "Type/FatalError", 452, TypeFatalError },
    { "Type/Warning", 453, TypeWarning },
    { "Wand/Error", 454, WandError },
    { "XServer/Error", 458, XServerError },
    { "XServer/FatalError", 480, XServerFatalError },
    { "XServer/Warning", 496, XServerWarning },
    { 0, 498, UndefinedException }
  };
#endif

#if defined(_INCLUDE_TAGMAP_TABLE_)
typedef struct _MessageInfo
{
  const char *name;
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
    { "UnableToAllocateCacheView", 12 },
    { "UnableToCloneCache", 13 },
    { "UnableToExtendCache", 14 },
    { "UnableToGetCacheNexus", 15 },
    { "UnableToGetPixelsFromCache", 16 },
    { "UnableToOpenCache", 17 },
    { "UnableToPeristPixelCache", 18 },
    { "UnableToReadPixelCache", 19 },
    { "UnableToSyncCache", 20 },
    { "DiskAllocationFailed", 21 },
    { "UnableToExtendPixelCache", 22 },
    { "Default", 23 },
    { "ColormapTooLarge", 24 },
    { "ColormapTypeNotSupported", 25 },
    { "ColorspaceModelIsNotSupported", 26 },
    { "ColorTypeNotSupported", 27 },
    { "CompressionNotValid", 28 },
    { "DataEncodingSchemeIsNotSupported", 29 },
    { "DataStorageTypeIsNotSupported", 30 },
    { "DeltaPNGNotSupported", 31 },
    { "EncryptedWPGImageFileNotSupported", 32 },
    { "FractalCompressionNotSupported", 33 },
    { "ImageColumnOrRowSizeIsNotSupported", 34 },
    { "ImageDoesNotHaveAMatteChannel", 35 },
    { "ImageIsNotTiled", 36 },
    { "ImageTypeNotSupported", 37 },
    { "IncompatibleSizeOfDouble", 38 },
    { "IrregularChannelGeometryNotSupported", 39 },
    { "JNGCompressionNotSupported", 40 },
    { "JPEGCompressionNotSupported", 41 },
    { "JPEGEmbeddingFailed", 42 },
    { "LocationTypeIsNotSupported", 43 },
    { "MapStorageTypeIsNotSupported", 44 },
    { "MSBByteOrderNotSupported", 45 },
    { "MultidimensionalMatricesAreNotSupported", 46 },
    { "MultipleRecordListNotSupported", 47 },
    { "No8BIMDataIsAvailable", 48 },
    { "NoAPP1DataIsAvailable", 49 },
    { "NoBitmapOnClipboard", 50 },
    { "NoColorProfileAvailable", 51 },
    { "NoDataReturned", 52 },
    { "NoImageVectorGraphics", 53 },
    { "NoIPTCInfoWasFound", 54 },
    { "NoIPTCProfileAvailable", 55 },
    { "NumberOfImagesIsNotSupported", 56 },
    { "OnlyContinuousTonePictureSupported", 57 },
    { "OnlyLevelZerofilesSupported", 58 },
    { "PNGCompressionNotSupported", 59 },
    { "PNGLibraryTooOld", 60 },
    { "RLECompressionNotSupported", 61 },
    { "SubsamplingRequiresEvenWidth", 62 },
    { "UnableToCopyProfile", 63 },
    { "UnableToCreateADC", 64 },
    { "UnableToCreateBitmap", 65 },
    { "UnableToDecompressImage", 66 },
    { "UnableToInitializeFPXLibrary", 67 },
    { "UnableToOpenBlob", 68 },
    { "UnableToReadAspectRatio", 69 },
    { "UnableToReadCIELABImages", 70 },
    { "UnableToReadSummaryInfo", 71 },
    { "UnableToSetAffineMatrix", 72 },
    { "UnableToSetAspectRatio", 73 },
    { "UnableToSetColorTwist", 74 },
    { "UnableToSetContrast", 75 },
    { "UnableToSetFilteringValue", 76 },
    { "UnableToSetImageComments", 77 },
    { "UnableToSetImageTitle", 78 },
    { "UnableToSetJPEGLevel", 79 },
    { "UnableToSetRegionOfInterest", 80 },
    { "UnableToSetSummaryInfo", 81 },
    { "UnableToTranslateText", 82 },
    { "UnableToWriteMPEGParameters", 83 },
    { "UnableToZipCompressImage", 84 },
    { "UnsupportedCellTypeInTheMatrix", 85 },
    { "ZipCompressionNotSupported", 86 },
    { "Default", 87 },
    { "LosslessToLossyJPEGConversion", 88 },
    { "IncludeElementNestedTooDeeply", 89 },
    { "RegistryKeyLookupFailed", 90 },
    { "UnableToAccessConfigureFile", 91 },
    { "UnableToAccessFontFile", 92 },
    { "UnableToAccessLogFile", 93 },
    { "UnableToAccessModuleFile", 94 },
    { "Default", 95 },
    { "Default", 96 },
    { "AnErrorHasOccurredReadingFromFile", 97 },
    { "AnErrorHasOccurredWritingToFile", 98 },
    { "ColormapExceeds256Colors", 99 },
    { "CorruptImage", 100 },
    { "ImageFileDoesNotContainAnyImageData", 101 },
    { "ImageTypeNotSupported", 102 },
    { "ImproperImageHeader", 103 },
    { "InsufficientImageDataInFile", 104 },
    { "InvalidColormapIndex", 105 },
    { "InvalidFileFormatVersion", 106 },
    { "LengthAndFilesizeDoNotMatch", 107 },
    { "MissingImageChannel", 108 },
    { "NegativeOrZeroImageSize", 109 },
    { "NonOS2HeaderSizeError", 110 },
    { "NotEnoughTiles", 111 },
    { "StaticPlanesValueNotEqualToOne", 112 },
    { "SubsamplingRequiresEvenWidth", 113 },
    { "TooMuchImageDataInFile", 114 },
    { "UnableToReadColormapFromDumpFile", 115 },
    { "UnableToReadColorProfile", 116 },
    { "UnableToReadExtensionBlock", 117 },
    { "UnableToReadGenericProfile", 118 },
    { "UnableToReadImageData", 119 },
    { "UnableToReadImageHeader", 120 },
    { "UnableToReadIPTCProfile", 121 },
    { "UnableToReadPixmapFromDumpFile", 122 },
    { "UnableToReadSubImageData", 123 },
    { "UnableToReadVIDImage", 124 },
    { "UnableToReadWindowNameFromDumpFile", 125 },
    { "UnableToRunlengthDecodeImage", 126 },
    { "UnableToUncompressImage", 127 },
    { "UnexpectedEndOfFile", 128 },
    { "UnexpectedSamplingFactor", 129 },
    { "UnknownPatternType", 130 },
    { "UnrecognizedBitsPerPixel", 131 },
    { "UnrecognizedImageCompression", 132 },
    { "UnrecognizedXWDHeader", 133 },
    { "UnableToPersistKey", 134 },
    { "CompressionNotValid", 135 },
    { "ImproperImageHeader", 136 },
    { "NegativeOrZeroImageSize", 137 },
    { "NonOS2HeaderSizeError", 138 },
    { "SkipToSyncByte", 139 },
    { "StaticPlanesValueNotEqualToOne", 140 },
    { "UnrecognizedBitsPerPixel", 141 },
    { "UnrecognizedImageCompression", 142 },
    { "UnrecognizedNumberOfColors", 143 },
    { "DelegateFailed", 144 },
    { "FailedToComputeOutputSize", 145 },
    { "FailedToRenderFile", 146 },
    { "FailedToScanFile", 147 },
    { "NoTagFound", 148 },
    { "PostscriptDelegateFailed", 149 },
    { "UnableToCreateImage", 150 },
    { "UnableToCreateImageComponent", 151 },
    { "UnableToDecodeImageFile", 152 },
    { "UnableToEncodeImageFile", 153 },
    { "UnableToInitializeFPXLibrary", 154 },
    { "UnableToInitializeWMFLibrary", 155 },
    { "UnableToManageJP2Stream", 156 },
    { "UnableToWriteSVGFormat", 157 },
    { "Default", 158 },
    { "Default", 159 },
    { "AlreadyPushingPatternDefinition", 160 },
    { "NonconformingDrawingPrimitiveDefinition", 161 },
    { "UnableToPrint", 162 },
    { "UnbalancedGraphicContextPushPop", 163 },
    { "Default", 164 },
    { "NotARelativeURL", 165 },
    { "NotCurrentlyPushingPatternDefinition", 166 },
    { "URLNotFound", 167 },
    { "UnableToCreateTemporaryFile", 168 },
    { "UnableToOpenFile", 169 },
    { "UnableToWriteFile", 170 },
    { "Default", 171 },
    { "Default", 172 },
    { "AngleIsDiscontinuous", 173 },
    { "ColorspaceColorProfileMismatch", 174 },
    { "ImageColorspaceDiffers", 175 },
    { "ImageDifferenceExceedsLimit", 176 },
    { "ImageDoesNotContainResolution", 177 },
    { "ImageOpacityDiffers", 178 },
    { "ImageSequenceIsRequired", 179 },
    { "ImageSizeDiffers", 180 },
    { "InvalidColormapIndex", 181 },
    { "LeftAndRightImageSizesDiffer", 182 },
    { "NoImagesWereFound", 183 },
    { "NoImagesWereLoaded", 184 },
    { "NoLocaleImageAttribute", 185 },
    { "TooManyClusters", 186 },
    { "UnableToAppendImage", 187 },
    { "UnableToAssignProfile", 188 },
    { "UnableToAverageImage", 189 },
    { "UnableToCoalesceImage", 190 },
    { "UnableToCompareImages", 191 },
    { "UnableToCreateImageMosaic", 192 },
    { "UnableToCreateStereoImage", 193 },
    { "UnableToDeconstructImageSequence", 194 },
    { "UnableToFlattenImage", 195 },
    { "UnableToGetClipMask", 196 },
    { "UnableToResizeImage", 197 },
    { "UnableToSegmentImage", 198 },
    { "UnableToSetClipMask", 199 },
    { "UnableToShearImage", 200 },
    { "WidthOrHeightExceedsLimit", 201 },
    { "UnableToPersistKey", 202 },
    { "Default", 203 },
    { "DPSLibraryIsNotAvailable", 204 },
    { "FPXLibraryIsNotAvailable", 205 },
    { "FreeTypeLibraryIsNotAvailable", 206 },
    { "JPEGLibraryIsNotAvailable", 207 },
    { "LCMSLibraryIsNotAvailable", 208 },
    { "LZWEncodingNotEnabled", 209 },
    { "NoDecodeDelegateForThisImageFormat", 210 },
    { "NoEncodeDelegateForThisImageFormat", 211 },
    { "TIFFLibraryIsNotAvailable", 212 },
    { "XMLLibraryIsNotAvailable", 213 },
    { "XWindowLibraryIsNotAvailable", 214 },
    { "ZipLibraryIsNotAvailable", 215 },
    { "Default", 216 },
    { "Default", 217 },
    { "FailedToCloseModule", 218 },
    { "FailedToFindSymbol", 219 },
    { "UnableToLoadModule", 220 },
    { "UnableToRegisterImageFormat", 221 },
    { "UnrecognizedModule", 222 },
    { "UnableToInitializeModuleLoader", 223 },
    { "Default", 224 },
    { "BevelWidthIsNegative", 225 },
    { "ColorSeparatedImageRequired", 226 },
    { "FrameIsLessThanImageSize", 227 },
    { "GeometryDimensionsAreZero", 228 },
    { "GeometryDoesNotContainImage", 229 },
    { "ImagesAreNotTheSameSize", 230 },
    { "ImageSizeMustExceedBevelWidth", 231 },
    { "ImageSmallerThanKernelWidth", 232 },
    { "ImageSmallerThanRadius", 233 },
    { "ImageWidthsOrHeightsDiffer", 234 },
    { "InputImagesAlreadySpecified", 235 },
    { "KernelRadiusIsTooSmall", 236 },
    { "KernelWidthMustBeAnOddNumber", 237 },
    { "MatrixOrderOutOfRange", 238 },
    { "MissingAnImageFilename", 239 },
    { "MissingArgument", 240 },
    { "MustSpecifyAnImageName", 241 },
    { "MustSpecifyImageSize", 242 },
    { "NoBlobDefined", 243 },
    { "NoImagesDefined", 244 },
    { "NonzeroWidthAndHeightRequired", 245 },
    { "NoProfileNameWasGiven", 246 },
    { "NullBlobArgument", 247 },
    { "ReferenceImageRequired", 248 },
    { "ReferenceIsNotMyType", 249 },
    { "RequestDidNotReturnAnImage", 250 },
    { "SteganoImageRequired", 251 },
    { "StereoImageRequired", 252 },
    { "SubimageSpecificationReturnsNoImages", 253 },
    { "UnableToAddOrRemoveProfile", 254 },
    { "UnableToAverageImageSequence", 255 },
    { "UnableToBlurImage", 256 },
    { "UnableToChopImage", 257 },
    { "UnableToColorMatrixImage", 258 },
    { "UnableToConstituteImage", 259 },
    { "UnableToConvolveImage", 260 },
    { "UnableToEdgeImage", 261 },
    { "UnableToEqualizeImage", 262 },
    { "UnableToFilterImage", 263 },
    { "UnableToFormatImageMetadata", 264 },
    { "UnableToFrameImage", 265 },
    { "UnableToOilPaintImage", 266 },
    { "UnableToPaintImage", 267 },
    { "UnableToRaiseImage", 268 },
    { "UnableToSharpenImage", 269 },
    { "UnableToThresholdImage", 270 },
    { "UnableToWaveImage", 271 },
    { "UnrecognizedAttribute", 272 },
    { "UnrecognizedChannelType", 273 },
    { "UnrecognizedColor", 274 },
    { "UnrecognizedColormapType", 275 },
    { "UnrecognizedColorspace", 276 },
    { "UnrecognizedCommand", 277 },
    { "UnrecognizedComposeOperator", 278 },
    { "UnrecognizedDisposeMethod", 279 },
    { "UnrecognizedElement", 280 },
    { "UnrecognizedEndianType", 281 },
    { "UnrecognizedGravityType", 282 },
    { "UnrecognizedHighlightStyle", 283 },
    { "UnrecognizedImageCompression", 284 },
    { "UnrecognizedImageFilter", 285 },
    { "UnrecognizedImageFormat", 286 },
    { "UnrecognizedImageMode", 287 },
    { "UnrecognizedImageType", 288 },
    { "UnrecognizedIntentType", 289 },
    { "UnrecognizedInterlaceType", 290 },
    { "UnrecognizedListType", 291 },
    { "UnrecognizedMetric", 292 },
    { "UnrecognizedModeType", 293 },
    { "UnrecognizedNoiseType", 294 },
    { "UnrecognizedOperator", 295 },
    { "UnrecognizedOption", 296 },
    { "UnrecognizedPerlMagickMethod", 297 },
    { "UnrecognizedPixelMap", 298 },
    { "UnrecognizedPreviewType", 299 },
    { "UnrecognizedResourceType", 300 },
    { "UnrecognizedType", 301 },
    { "UnrecognizedUnitsType", 302 },
    { "UnrecognizedVirtualPixelMethod", 303 },
    { "UnsupportedSamplingFactor", 304 },
    { "UsageError", 305 },
    { "InvalidColorspaceType", 306 },
    { "InvalidEndianType", 307 },
    { "InvalidImageType", 308 },
    { "InvalidInterlaceType", 309 },
    { "MissingAnImageFilename", 310 },
    { "MissingArgument", 311 },
    { "NoImagesWereLoaded", 312 },
    { "OptionLengthExceedsLimit", 313 },
    { "RequestDidNotReturnAnImage", 314 },
    { "UnableToOpenXServer", 315 },
    { "UnableToPersistKey", 316 },
    { "UnrecognizedColormapType", 317 },
    { "UnrecognizedColorspaceType", 318 },
    { "UnrecognizedDisposeMethod", 319 },
    { "UnrecognizedEndianType", 320 },
    { "UnrecognizedFilterType", 321 },
    { "UnrecognizedImageCompressionType", 322 },
    { "UnrecognizedImageType", 323 },
    { "UnrecognizedInterlaceType", 324 },
    { "UnrecognizedOption", 325 },
    { "UnrecognizedResourceType", 326 },
    { "UnrecognizedVirtualPixelMethod", 327 },
    { "UnrecognizedColor", 328 },
    { "ImageExpected", 329 },
    { "ImageInfoExpected", 330 },
    { "StructureSizeMismatch", 331 },
    { "UnableToGetRegistryID", 332 },
    { "UnableToLocateImage", 333 },
    { "UnableToSetRegistry", 334 },
    { "Default", 335 },
    { "Default", 336 },
    { "CacheResourcesExhausted", 337 },
    { "ImagePixelLimitExceeded", 338 },
    { "MemoryAllocationFailed", 339 },
    { "NoPixelsDefinedInCache", 340 },
    { "PixelCacheAllocationFailed", 341 },
    { "UnableToAddColorProfile", 342 },
    { "UnableToAddGenericProfile", 343 },
    { "UnableToAddIPTCProfile", 344 },
    { "UnableToAllocateCoefficients", 345 },
    { "UnableToAllocateColormap", 346 },
    { "UnableToAllocateICCProfile", 347 },
    { "UnableToAllocateString", 348 },
    { "UnableToAnnotateImage", 349 },
    { "UnableToAverageImageSequence", 350 },
    { "UnableToCloneImage", 351 },
    { "UnableToComputeImageSignature", 352 },
    { "UnableToConstituteImage", 353 },
    { "UnableToConvertFont", 354 },
    { "UnableToConvertStringToTokens", 355 },
    { "UnableToCreateColormap", 356 },
    { "UnableToCreateColorTransform", 357 },
    { "UnableToCreateCommandWidget", 358 },
    { "UnableToCreateImageGroup", 359 },
    { "UnableToCreateImageMontage", 360 },
    { "UnableToCreateXWindow", 361 },
    { "UnableToCropImage", 362 },
    { "UnableToDespeckleImage", 363 },
    { "UnableToDetermineImageClass", 364 },
    { "UnableToDetermineTheNumberOfImageColors", 365 },
    { "UnableToDitherImage", 366 },
    { "UnableToDrawOnImage", 367 },
    { "UnableToEdgeImage", 368 },
    { "UnableToEmbossImage", 369 },
    { "UnableToEnhanceImage", 370 },
    { "UnableToFloodfillImage", 371 },
    { "UnableToGammaCorrectImage", 372 },
    { "UnableToGetBestIconSize", 373 },
    { "UnableToGetFromRegistry", 374 },
    { "UnableToGetPackageInfo", 375 },
    { "UnableToLevelImage", 376 },
    { "UnableToMagnifyImage", 377 },
    { "UnableToManageColor", 378 },
    { "UnableToMapImage", 379 },
    { "UnableToMapImageSequence", 380 },
    { "UnableToMedianFilterImage", 381 },
    { "UnableToMotionBlurImage", 382 },
    { "UnableToNoiseFilterImage", 383 },
    { "UnableToNormalizeImage", 384 },
    { "UnableToOpenColorProfile", 385 },
    { "UnableToQuantizeImage", 386 },
    { "UnableToQuantizeImageSequence", 387 },
    { "UnableToReadTextChunk", 388 },
    { "UnableToReadXImage", 389 },
    { "UnableToReadXServerColormap", 390 },
    { "UnableToResizeImage", 391 },
    { "UnableToRotateImage", 392 },
    { "UnableToSampleImage", 393 },
    { "UnableToScaleImage", 394 },
    { "UnableToSelectImage", 395 },
    { "UnableToSharpenImage", 396 },
    { "UnableToShaveImage", 397 },
    { "UnableToShearImage", 398 },
    { "UnableToSortImageColormap", 399 },
    { "UnableToThresholdImage", 400 },
    { "UnableToTransformColorspace", 401 },
    { "MemoryAllocationFailed", 402 },
    { "UnableToAllocateAscii85Info", 403 },
    { "UnableToAllocateCacheInfo", 404 },
    { "UnableToAllocateCacheView", 405 },
    { "UnableToAllocateColorInfo", 406 },
    { "UnableToAllocateDashPattern", 407 },
    { "UnableToAllocateDelegateInfo", 408 },
    { "UnableToAllocateDerivatives", 409 },
    { "UnableToAllocateDrawContext", 410 },
    { "UnableToAllocateDrawInfo", 411 },
    { "UnableToAllocateDrawingWand", 412 },
    { "UnableToAllocateGammaMap", 413 },
    { "UnableToAllocateImage", 414 },
    { "UnableToAllocateImagePixels", 415 },
    { "UnableToAllocateLogInfo", 416 },
    { "UnableToAllocateMagicInfo", 417 },
    { "UnableToAllocateMagickInfo", 418 },
    { "UnableToAllocateModuleInfo", 419 },
    { "UnableToAllocateMontageInfo", 420 },
    { "UnableToAllocateQuantizeInfo", 421 },
    { "UnableToAllocateRandomKernel", 422 },
    { "UnableToAllocateRegistryInfo", 423 },
    { "UnableToAllocateSemaphoreInfo", 424 },
    { "UnableToAllocateString", 425 },
    { "UnableToAllocateTypeInfo", 426 },
    { "UnableToAllocateWand", 427 },
    { "UnableToAnimateImageSequence", 428 },
    { "UnableToCloneBlobInfo", 429 },
    { "UnableToCloneCacheInfo", 430 },
    { "UnableToCloneImage", 431 },
    { "UnableToCloneImageInfo", 432 },
    { "UnableToConcatenateString", 433 },
    { "UnableToConvertText", 434 },
    { "UnableToCreateColormap", 435 },
    { "UnableToDisplayImage", 436 },
    { "UnableToEscapeString", 437 },
    { "UnableToInterpretMSLImage", 438 },
    { "MemoryAllocationFailed", 439 },
    { "ImageDoesNotContainTheStreamGeometry", 440 },
    { "NoStreamHandlerIsDefined", 441 },
    { "PixelCacheIsNotOpen", 442 },
    { "UnableToAcquirePixelStream", 443 },
    { "UnableToSetPixelStream", 444 },
    { "UnableToSyncPixelStream", 445 },
    { "Default", 446 },
    { "Default", 447 },
    { "FontSubstitutionRequired", 448 },
    { "UnableToGetTypeMetrics", 449 },
    { "UnableToInitializeFreetypeLibrary", 450 },
    { "UnableToReadFont", 451 },
    { "UnrecognizedFontEncoding", 452 },
    { "Default", 453 },
    { "Default", 454 },
    { "InvalidColormapIndex", 455 },
    { "WandAPINotImplemented", 456 },
    { "WandContainsNoImageIndexs", 457 },
    { "WandContainsNoImages", 458 },
    { "ColorIsNotKnownToServer", 459 },
    { "NoWindowWithSpecifiedIDExists", 460 },
    { "StandardColormapIsNotInitialized", 461 },
    { "UnableToConnectToRemoteDisplay", 462 },
    { "UnableToCreateBitmap", 463 },
    { "UnableToCreateColormap", 464 },
    { "UnableToCreatePixmap", 465 },
    { "UnableToCreateProperty", 466 },
    { "UnableToCreateStandardColormap", 467 },
    { "UnableToDisplayImageInfo", 468 },
    { "UnableToGetProperty", 469 },
    { "UnableToGetStandardColormap", 470 },
    { "UnableToGetVisual", 471 },
    { "UnableToGrabMouse", 472 },
    { "UnableToLoadFont", 473 },
    { "UnableToMatchVisualToStandardColormap", 474 },
    { "UnableToOpenXServer", 475 },
    { "UnableToReadXAttributes", 476 },
    { "UnableToReadXWindowImage", 477 },
    { "UnrecognizedColormapType", 478 },
    { "UnrecognizedGravityType", 479 },
    { "UnrecognizedVisualSpecifier", 480 },
    { "UnableToAllocateXHints", 481 },
    { "UnableToCreateCursor", 482 },
    { "UnableToCreateGraphicContext", 483 },
    { "UnableToCreateStandardColormap", 484 },
    { "UnableToCreateTextProperty", 485 },
    { "UnableToCreateXImage", 486 },
    { "UnableToCreateXPixmap", 487 },
    { "UnableToCreateXWindow", 488 },
    { "UnableToDisplayImage", 489 },
    { "UnableToDitherImage", 490 },
    { "UnableToGetPixelInfo", 491 },
    { "UnableToGetVisual", 492 },
    { "UnableToLoadFont", 493 },
    { "UnableToMakeXWindow", 494 },
    { "UnableToOpenXServer", 495 },
    { "UnableToViewFonts", 496 },
    { "UnableToGetVisual", 497 },
    { "UsingDefaultVisual", 498 },
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
    "Unable to allocate cache view",
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
    "Subsampling requires that image width be evenly divisible by two",
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
    "Subsampling requires that image width be evenly divisible by two",
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
    "image difference exceeds limit (%s)",
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
    "Matrix size is out of range",
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
    "Request did not return an image",
    "Stegano image required",
    "Stereo image required",
    "Subimage specification returns no images",
    "Unable to add or remove profile",
    "unable to average image sequence",
    "unable to blur image",
    "unable to chop image",
    "Unable to color matrix image",
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
    "Unrecognized highlight style",
    "Unrecognized image compression",
    "Unrecognized image filter",
    "Unrecognized image format",
    "Unrecognized image mode",
    "Unrecognized image type",
    "Unrecognized intent type",
    "Unrecognized interlace type",
    "Unrecognized list type",
    "Unrecognized error metric",
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
    "Unsupported sampling factor",
    "Improper arguments supplied, please see manual",
    "Invalid colorspace type",
    "Invalid endian type",
    "Invalid image type",
    "Invalid interlace type",
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
    "No images were loaded",
    "Option length exceeds limit",
    "Request did not return an image",
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
    "Disk space limit exceeded (see -limit Disk)",
    "Image pixel limit exceeded (see -limit Pixels)",
    "Memory allocation failed",
    "No pixels defined in cache",
    "Pixel cache allocation failed",
    "unable to add ICC Color profile",
    "unable to add generic profile",
    "unable to add IPTC profile",
    "unable to allocate coefficients",
    "Unable to allocate colormap",
    "unable to allocate ICC profile",
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
    "unable to allocate random kernel",
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
