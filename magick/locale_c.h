#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 502

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
#define MGK_DelegateErrorFailedToAllocateArgumentList 145
#define MGK_DelegateErrorFailedToAllocateGhostscriptInterpreter 146
#define MGK_DelegateErrorFailedToComputeOutputSize 147
#define MGK_DelegateErrorFailedToRenderFile 148
#define MGK_DelegateErrorFailedToScanFile 149
#define MGK_DelegateErrorNoTagFound 150
#define MGK_DelegateErrorPostscriptDelegateFailed 151
#define MGK_DelegateErrorUnableToCreateImage 152
#define MGK_DelegateErrorUnableToCreateImageComponent 153
#define MGK_DelegateErrorUnableToDecodeImageFile 154
#define MGK_DelegateErrorUnableToEncodeImageFile 155
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 156
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 157
#define MGK_DelegateErrorUnableToManageJP2Stream 158
#define MGK_DelegateErrorUnableToWriteSVGFormat 159
#define MGK_DelegateFatalErrorDefault 160
#define MGK_DelegateWarningDefault 161
#define MGK_DrawErrorAlreadyPushingPatternDefinition 162
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 163
#define MGK_DrawErrorUnableToPrint 164
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 165
#define MGK_DrawFatalErrorDefault 166
#define MGK_DrawWarningNotARelativeURL 167
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 168
#define MGK_DrawWarningURLNotFound 169
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 170
#define MGK_FileOpenErrorUnableToOpenFile 171
#define MGK_FileOpenErrorUnableToWriteFile 172
#define MGK_FileOpenFatalErrorDefault 173
#define MGK_FileOpenWarningDefault 174
#define MGK_ImageErrorAngleIsDiscontinuous 175
#define MGK_ImageErrorColorspaceColorProfileMismatch 176
#define MGK_ImageErrorImageColorspaceDiffers 177
#define MGK_ImageErrorImageDifferenceExceedsLimit 178
#define MGK_ImageErrorImageDoesNotContainResolution 179
#define MGK_ImageErrorImageOpacityDiffers 180
#define MGK_ImageErrorImageSequenceIsRequired 181
#define MGK_ImageErrorImageSizeDiffers 182
#define MGK_ImageErrorInvalidColormapIndex 183
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 184
#define MGK_ImageErrorNoImagesWereFound 185
#define MGK_ImageErrorNoImagesWereLoaded 186
#define MGK_ImageErrorNoLocaleImageAttribute 187
#define MGK_ImageErrorTooManyClusters 188
#define MGK_ImageErrorUnableToAppendImage 189
#define MGK_ImageErrorUnableToAssignProfile 190
#define MGK_ImageErrorUnableToAverageImage 191
#define MGK_ImageErrorUnableToCoalesceImage 192
#define MGK_ImageErrorUnableToCompareImages 193
#define MGK_ImageErrorUnableToCreateImageMosaic 194
#define MGK_ImageErrorUnableToCreateStereoImage 195
#define MGK_ImageErrorUnableToDeconstructImageSequence 196
#define MGK_ImageErrorUnableToFlattenImage 197
#define MGK_ImageErrorUnableToGetClipMask 198
#define MGK_ImageErrorUnableToResizeImage 199
#define MGK_ImageErrorUnableToSegmentImage 200
#define MGK_ImageErrorUnableToSetClipMask 201
#define MGK_ImageErrorUnableToShearImage 202
#define MGK_ImageErrorWidthOrHeightExceedsLimit 203
#define MGK_ImageFatalErrorUnableToPersistKey 204
#define MGK_ImageWarningDefault 205
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 206
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 207
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 208
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 209
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 210
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 211
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 212
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 213
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 214
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 215
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 216
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 217
#define MGK_MissingDelegateFatalErrorDefault 218
#define MGK_MissingDelegateWarningDefault 219
#define MGK_ModuleErrorFailedToCloseModule 220
#define MGK_ModuleErrorFailedToFindSymbol 221
#define MGK_ModuleErrorUnableToLoadModule 222
#define MGK_ModuleErrorUnableToRegisterImageFormat 223
#define MGK_ModuleErrorUnrecognizedModule 224
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 225
#define MGK_ModuleWarningDefault 226
#define MGK_OptionErrorBevelWidthIsNegative 227
#define MGK_OptionErrorColorSeparatedImageRequired 228
#define MGK_OptionErrorFrameIsLessThanImageSize 229
#define MGK_OptionErrorGeometryDimensionsAreZero 230
#define MGK_OptionErrorGeometryDoesNotContainImage 231
#define MGK_OptionErrorHaldClutImageDimensionsInvalid 232
#define MGK_OptionErrorImagesAreNotTheSameSize 233
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 234
#define MGK_OptionErrorImageSmallerThanKernelWidth 235
#define MGK_OptionErrorImageSmallerThanRadius 236
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 237
#define MGK_OptionErrorInputImagesAlreadySpecified 238
#define MGK_OptionErrorKernelRadiusIsTooSmall 239
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 240
#define MGK_OptionErrorMatrixOrderOutOfRange 241
#define MGK_OptionErrorMissingAnImageFilename 242
#define MGK_OptionErrorMissingArgument 243
#define MGK_OptionErrorMustSpecifyAnImageName 244
#define MGK_OptionErrorMustSpecifyImageSize 245
#define MGK_OptionErrorNoBlobDefined 246
#define MGK_OptionErrorNoImagesDefined 247
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 248
#define MGK_OptionErrorNoProfileNameWasGiven 249
#define MGK_OptionErrorNullBlobArgument 250
#define MGK_OptionErrorReferenceImageRequired 251
#define MGK_OptionErrorReferenceIsNotMyType 252
#define MGK_OptionErrorRequestDidNotReturnAnImage 253
#define MGK_OptionErrorSteganoImageRequired 254
#define MGK_OptionErrorStereoImageRequired 255
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 256
#define MGK_OptionErrorUnableToAddOrRemoveProfile 257
#define MGK_OptionErrorUnableToAverageImageSequence 258
#define MGK_OptionErrorUnableToBlurImage 259
#define MGK_OptionErrorUnableToChopImage 260
#define MGK_OptionErrorUnableToColorMatrixImage 261
#define MGK_OptionErrorUnableToConstituteImage 262
#define MGK_OptionErrorUnableToConvolveImage 263
#define MGK_OptionErrorUnableToEdgeImage 264
#define MGK_OptionErrorUnableToEqualizeImage 265
#define MGK_OptionErrorUnableToFilterImage 266
#define MGK_OptionErrorUnableToFormatImageMetadata 267
#define MGK_OptionErrorUnableToFrameImage 268
#define MGK_OptionErrorUnableToOilPaintImage 269
#define MGK_OptionErrorUnableToPaintImage 270
#define MGK_OptionErrorUnableToRaiseImage 271
#define MGK_OptionErrorUnableToSharpenImage 272
#define MGK_OptionErrorUnableToThresholdImage 273
#define MGK_OptionErrorUnableToWaveImage 274
#define MGK_OptionErrorUnrecognizedAttribute 275
#define MGK_OptionErrorUnrecognizedChannelType 276
#define MGK_OptionErrorUnrecognizedColor 277
#define MGK_OptionErrorUnrecognizedColormapType 278
#define MGK_OptionErrorUnrecognizedColorspace 279
#define MGK_OptionErrorUnrecognizedCommand 280
#define MGK_OptionErrorUnrecognizedComposeOperator 281
#define MGK_OptionErrorUnrecognizedDisposeMethod 282
#define MGK_OptionErrorUnrecognizedElement 283
#define MGK_OptionErrorUnrecognizedEndianType 284
#define MGK_OptionErrorUnrecognizedGravityType 285
#define MGK_OptionErrorUnrecognizedHighlightStyle 286
#define MGK_OptionErrorUnrecognizedImageCompression 287
#define MGK_OptionErrorUnrecognizedImageFilter 288
#define MGK_OptionErrorUnrecognizedImageFormat 289
#define MGK_OptionErrorUnrecognizedImageMode 290
#define MGK_OptionErrorUnrecognizedImageType 291
#define MGK_OptionErrorUnrecognizedIntentType 292
#define MGK_OptionErrorUnrecognizedInterlaceType 293
#define MGK_OptionErrorUnrecognizedListType 294
#define MGK_OptionErrorUnrecognizedMetric 295
#define MGK_OptionErrorUnrecognizedModeType 296
#define MGK_OptionErrorUnrecognizedNoiseType 297
#define MGK_OptionErrorUnrecognizedOperator 298
#define MGK_OptionErrorUnrecognizedOption 299
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 300
#define MGK_OptionErrorUnrecognizedPixelMap 301
#define MGK_OptionErrorUnrecognizedPreviewType 302
#define MGK_OptionErrorUnrecognizedResourceType 303
#define MGK_OptionErrorUnrecognizedType 304
#define MGK_OptionErrorUnrecognizedUnitsType 305
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 306
#define MGK_OptionErrorUnsupportedSamplingFactor 307
#define MGK_OptionErrorUsageError 308
#define MGK_OptionFatalErrorInvalidColorspaceType 309
#define MGK_OptionFatalErrorInvalidEndianType 310
#define MGK_OptionFatalErrorInvalidImageType 311
#define MGK_OptionFatalErrorInvalidInterlaceType 312
#define MGK_OptionFatalErrorMissingAnImageFilename 313
#define MGK_OptionFatalErrorMissingArgument 314
#define MGK_OptionFatalErrorNoImagesWereLoaded 315
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 316
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 317
#define MGK_OptionFatalErrorUnableToOpenXServer 318
#define MGK_OptionFatalErrorUnableToPersistKey 319
#define MGK_OptionFatalErrorUnrecognizedColormapType 320
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 321
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 322
#define MGK_OptionFatalErrorUnrecognizedEndianType 323
#define MGK_OptionFatalErrorUnrecognizedFilterType 324
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 325
#define MGK_OptionFatalErrorUnrecognizedImageType 326
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 327
#define MGK_OptionFatalErrorUnrecognizedOption 328
#define MGK_OptionFatalErrorUnrecognizedResourceType 329
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 330
#define MGK_OptionWarningUnrecognizedColor 331
#define MGK_RegistryErrorImageExpected 332
#define MGK_RegistryErrorImageInfoExpected 333
#define MGK_RegistryErrorStructureSizeMismatch 334
#define MGK_RegistryErrorUnableToGetRegistryID 335
#define MGK_RegistryErrorUnableToLocateImage 336
#define MGK_RegistryErrorUnableToSetRegistry 337
#define MGK_RegistryFatalErrorDefault 338
#define MGK_RegistryWarningDefault 339
#define MGK_ResourceLimitErrorCacheResourcesExhausted 340
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 341
#define MGK_ResourceLimitErrorMemoryAllocationFailed 342
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 343
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 344
#define MGK_ResourceLimitErrorUnableToAddColorProfile 345
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 346
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 347
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 348
#define MGK_ResourceLimitErrorUnableToAllocateColormap 349
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 350
#define MGK_ResourceLimitErrorUnableToAllocateString 351
#define MGK_ResourceLimitErrorUnableToAnnotateImage 352
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 353
#define MGK_ResourceLimitErrorUnableToCloneDrawingWand 354
#define MGK_ResourceLimitErrorUnableToCloneImage 355
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 356
#define MGK_ResourceLimitErrorUnableToConstituteImage 357
#define MGK_ResourceLimitErrorUnableToConvertFont 358
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 359
#define MGK_ResourceLimitErrorUnableToCreateColormap 360
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 361
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 362
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 363
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 364
#define MGK_ResourceLimitErrorUnableToCreateXWindow 365
#define MGK_ResourceLimitErrorUnableToCropImage 366
#define MGK_ResourceLimitErrorUnableToDespeckleImage 367
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 368
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 369
#define MGK_ResourceLimitErrorUnableToDitherImage 370
#define MGK_ResourceLimitErrorUnableToDrawOnImage 371
#define MGK_ResourceLimitErrorUnableToEdgeImage 372
#define MGK_ResourceLimitErrorUnableToEmbossImage 373
#define MGK_ResourceLimitErrorUnableToEnhanceImage 374
#define MGK_ResourceLimitErrorUnableToFloodfillImage 375
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 376
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 377
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 378
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 379
#define MGK_ResourceLimitErrorUnableToLevelImage 380
#define MGK_ResourceLimitErrorUnableToMagnifyImage 381
#define MGK_ResourceLimitErrorUnableToManageColor 382
#define MGK_ResourceLimitErrorUnableToMapImage 383
#define MGK_ResourceLimitErrorUnableToMapImageSequence 384
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 385
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 386
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 387
#define MGK_ResourceLimitErrorUnableToNormalizeImage 388
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 389
#define MGK_ResourceLimitErrorUnableToQuantizeImage 390
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 391
#define MGK_ResourceLimitErrorUnableToReadTextChunk 392
#define MGK_ResourceLimitErrorUnableToReadXImage 393
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 394
#define MGK_ResourceLimitErrorUnableToResizeImage 395
#define MGK_ResourceLimitErrorUnableToRotateImage 396
#define MGK_ResourceLimitErrorUnableToSampleImage 397
#define MGK_ResourceLimitErrorUnableToScaleImage 398
#define MGK_ResourceLimitErrorUnableToSelectImage 399
#define MGK_ResourceLimitErrorUnableToSharpenImage 400
#define MGK_ResourceLimitErrorUnableToShaveImage 401
#define MGK_ResourceLimitErrorUnableToShearImage 402
#define MGK_ResourceLimitErrorUnableToSortImageColormap 403
#define MGK_ResourceLimitErrorUnableToThresholdImage 404
#define MGK_ResourceLimitErrorUnableToTransformColorspace 405
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 406
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 407
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 408
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 409
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 410
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 411
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 412
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 413
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 414
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 415
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 416
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 417
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 418
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 419
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 420
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 421
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 422
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 423
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 424
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 425
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 426
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 427
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 428
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 429
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 430
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 431
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 432
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 433
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 434
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 435
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 436
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 437
#define MGK_ResourceLimitFatalErrorUnableToConvertText 438
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 439
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 440
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 441
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 442
#define MGK_ResourceLimitWarningMemoryAllocationFailed 443
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 444
#define MGK_StreamErrorNoStreamHandlerIsDefined 445
#define MGK_StreamErrorPixelCacheIsNotOpen 446
#define MGK_StreamErrorUnableToAcquirePixelStream 447
#define MGK_StreamErrorUnableToSetPixelStream 448
#define MGK_StreamErrorUnableToSyncPixelStream 449
#define MGK_StreamFatalErrorDefault 450
#define MGK_StreamWarningDefault 451
#define MGK_TypeErrorFontSubstitutionRequired 452
#define MGK_TypeErrorUnableToGetTypeMetrics 453
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 454
#define MGK_TypeErrorUnableToReadFont 455
#define MGK_TypeErrorUnrecognizedFontEncoding 456
#define MGK_TypeFatalErrorDefault 457
#define MGK_TypeWarningDefault 458
#define MGK_WandErrorInvalidColormapIndex 459
#define MGK_WandErrorWandAPINotImplemented 460
#define MGK_WandErrorWandContainsNoImageIndexs 461
#define MGK_WandErrorWandContainsNoImages 462
#define MGK_XServerErrorColorIsNotKnownToServer 463
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 464
#define MGK_XServerErrorStandardColormapIsNotInitialized 465
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 466
#define MGK_XServerErrorUnableToCreateBitmap 467
#define MGK_XServerErrorUnableToCreateColormap 468
#define MGK_XServerErrorUnableToCreatePixmap 469
#define MGK_XServerErrorUnableToCreateProperty 470
#define MGK_XServerErrorUnableToCreateStandardColormap 471
#define MGK_XServerErrorUnableToDisplayImageInfo 472
#define MGK_XServerErrorUnableToGetProperty 473
#define MGK_XServerErrorUnableToGetStandardColormap 474
#define MGK_XServerErrorUnableToGetVisual 475
#define MGK_XServerErrorUnableToGrabMouse 476
#define MGK_XServerErrorUnableToLoadFont 477
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 478
#define MGK_XServerErrorUnableToOpenXServer 479
#define MGK_XServerErrorUnableToReadXAttributes 480
#define MGK_XServerErrorUnableToReadXWindowImage 481
#define MGK_XServerErrorUnrecognizedColormapType 482
#define MGK_XServerErrorUnrecognizedGravityType 483
#define MGK_XServerErrorUnrecognizedVisualSpecifier 484
#define MGK_XServerFatalErrorUnableToAllocateXHints 485
#define MGK_XServerFatalErrorUnableToCreateCursor 486
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 487
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 488
#define MGK_XServerFatalErrorUnableToCreateTextProperty 489
#define MGK_XServerFatalErrorUnableToCreateXImage 490
#define MGK_XServerFatalErrorUnableToCreateXPixmap 491
#define MGK_XServerFatalErrorUnableToCreateXWindow 492
#define MGK_XServerFatalErrorUnableToDisplayImage 493
#define MGK_XServerFatalErrorUnableToDitherImage 494
#define MGK_XServerFatalErrorUnableToGetPixelInfo 495
#define MGK_XServerFatalErrorUnableToGetVisual 496
#define MGK_XServerFatalErrorUnableToLoadFont 497
#define MGK_XServerFatalErrorUnableToMakeXWindow 498
#define MGK_XServerFatalErrorUnableToOpenXServer 499
#define MGK_XServerFatalErrorUnableToViewFonts 500
#define MGK_XServerWarningUnableToGetVisual 501
#define MGK_XServerWarningUsingDefaultVisual 502

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
    { "Delegate/FatalError", 159, DelegateFatalError },
    { "Delegate/Warning", 160, DelegateWarning },
    { "Draw/Error", 161, DrawError },
    { "Draw/FatalError", 165, DrawFatalError },
    { "Draw/Warning", 166, DrawWarning },
    { "File/Open/Error", 169, FileOpenError },
    { "File/Open/FatalError", 172, FileOpenFatalError },
    { "File/Open/Warning", 173, FileOpenWarning },
    { "Image/Error", 174, ImageError },
    { "Image/FatalError", 203, ImageFatalError },
    { "Image/Warning", 204, ImageWarning },
    { "Missing/Delegate/Error", 205, MissingDelegateError },
    { "Missing/Delegate/FatalError", 217, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 218, MissingDelegateWarning },
    { "Module/Error", 219, ModuleError },
    { "Module/FatalError", 224, ModuleFatalError },
    { "Module/Warning", 225, ModuleWarning },
    { "Option/Error", 226, OptionError },
    { "Option/FatalError", 308, OptionFatalError },
    { "Option/Warning", 330, OptionWarning },
    { "Registry/Error", 331, RegistryError },
    { "Registry/FatalError", 337, RegistryFatalError },
    { "Registry/Warning", 338, RegistryWarning },
    { "Resource/Limit/Error", 339, ResourceLimitError },
    { "Resource/Limit/FatalError", 405, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 442, ResourceLimitWarning },
    { "Stream/Error", 443, StreamError },
    { "Stream/FatalError", 449, StreamFatalError },
    { "Stream/Warning", 450, StreamWarning },
    { "Type/Error", 451, TypeError },
    { "Type/FatalError", 456, TypeFatalError },
    { "Type/Warning", 457, TypeWarning },
    { "Wand/Error", 458, WandError },
    { "XServer/Error", 462, XServerError },
    { "XServer/FatalError", 484, XServerFatalError },
    { "XServer/Warning", 500, XServerWarning },
    { 0, 502, UndefinedException }
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
    { "FailedToAllocateArgumentList", 145 },
    { "FailedToAllocateGhostscriptInterpreter", 146 },
    { "FailedToComputeOutputSize", 147 },
    { "FailedToRenderFile", 148 },
    { "FailedToScanFile", 149 },
    { "NoTagFound", 150 },
    { "PostscriptDelegateFailed", 151 },
    { "UnableToCreateImage", 152 },
    { "UnableToCreateImageComponent", 153 },
    { "UnableToDecodeImageFile", 154 },
    { "UnableToEncodeImageFile", 155 },
    { "UnableToInitializeFPXLibrary", 156 },
    { "UnableToInitializeWMFLibrary", 157 },
    { "UnableToManageJP2Stream", 158 },
    { "UnableToWriteSVGFormat", 159 },
    { "Default", 160 },
    { "Default", 161 },
    { "AlreadyPushingPatternDefinition", 162 },
    { "NonconformingDrawingPrimitiveDefinition", 163 },
    { "UnableToPrint", 164 },
    { "UnbalancedGraphicContextPushPop", 165 },
    { "Default", 166 },
    { "NotARelativeURL", 167 },
    { "NotCurrentlyPushingPatternDefinition", 168 },
    { "URLNotFound", 169 },
    { "UnableToCreateTemporaryFile", 170 },
    { "UnableToOpenFile", 171 },
    { "UnableToWriteFile", 172 },
    { "Default", 173 },
    { "Default", 174 },
    { "AngleIsDiscontinuous", 175 },
    { "ColorspaceColorProfileMismatch", 176 },
    { "ImageColorspaceDiffers", 177 },
    { "ImageDifferenceExceedsLimit", 178 },
    { "ImageDoesNotContainResolution", 179 },
    { "ImageOpacityDiffers", 180 },
    { "ImageSequenceIsRequired", 181 },
    { "ImageSizeDiffers", 182 },
    { "InvalidColormapIndex", 183 },
    { "LeftAndRightImageSizesDiffer", 184 },
    { "NoImagesWereFound", 185 },
    { "NoImagesWereLoaded", 186 },
    { "NoLocaleImageAttribute", 187 },
    { "TooManyClusters", 188 },
    { "UnableToAppendImage", 189 },
    { "UnableToAssignProfile", 190 },
    { "UnableToAverageImage", 191 },
    { "UnableToCoalesceImage", 192 },
    { "UnableToCompareImages", 193 },
    { "UnableToCreateImageMosaic", 194 },
    { "UnableToCreateStereoImage", 195 },
    { "UnableToDeconstructImageSequence", 196 },
    { "UnableToFlattenImage", 197 },
    { "UnableToGetClipMask", 198 },
    { "UnableToResizeImage", 199 },
    { "UnableToSegmentImage", 200 },
    { "UnableToSetClipMask", 201 },
    { "UnableToShearImage", 202 },
    { "WidthOrHeightExceedsLimit", 203 },
    { "UnableToPersistKey", 204 },
    { "Default", 205 },
    { "DPSLibraryIsNotAvailable", 206 },
    { "FPXLibraryIsNotAvailable", 207 },
    { "FreeTypeLibraryIsNotAvailable", 208 },
    { "JPEGLibraryIsNotAvailable", 209 },
    { "LCMSLibraryIsNotAvailable", 210 },
    { "LZWEncodingNotEnabled", 211 },
    { "NoDecodeDelegateForThisImageFormat", 212 },
    { "NoEncodeDelegateForThisImageFormat", 213 },
    { "TIFFLibraryIsNotAvailable", 214 },
    { "XMLLibraryIsNotAvailable", 215 },
    { "XWindowLibraryIsNotAvailable", 216 },
    { "ZipLibraryIsNotAvailable", 217 },
    { "Default", 218 },
    { "Default", 219 },
    { "FailedToCloseModule", 220 },
    { "FailedToFindSymbol", 221 },
    { "UnableToLoadModule", 222 },
    { "UnableToRegisterImageFormat", 223 },
    { "UnrecognizedModule", 224 },
    { "UnableToInitializeModuleLoader", 225 },
    { "Default", 226 },
    { "BevelWidthIsNegative", 227 },
    { "ColorSeparatedImageRequired", 228 },
    { "FrameIsLessThanImageSize", 229 },
    { "GeometryDimensionsAreZero", 230 },
    { "GeometryDoesNotContainImage", 231 },
    { "HaldClutImageDimensionsInvalid", 232 },
    { "ImagesAreNotTheSameSize", 233 },
    { "ImageSizeMustExceedBevelWidth", 234 },
    { "ImageSmallerThanKernelWidth", 235 },
    { "ImageSmallerThanRadius", 236 },
    { "ImageWidthsOrHeightsDiffer", 237 },
    { "InputImagesAlreadySpecified", 238 },
    { "KernelRadiusIsTooSmall", 239 },
    { "KernelWidthMustBeAnOddNumber", 240 },
    { "MatrixOrderOutOfRange", 241 },
    { "MissingAnImageFilename", 242 },
    { "MissingArgument", 243 },
    { "MustSpecifyAnImageName", 244 },
    { "MustSpecifyImageSize", 245 },
    { "NoBlobDefined", 246 },
    { "NoImagesDefined", 247 },
    { "NonzeroWidthAndHeightRequired", 248 },
    { "NoProfileNameWasGiven", 249 },
    { "NullBlobArgument", 250 },
    { "ReferenceImageRequired", 251 },
    { "ReferenceIsNotMyType", 252 },
    { "RequestDidNotReturnAnImage", 253 },
    { "SteganoImageRequired", 254 },
    { "StereoImageRequired", 255 },
    { "SubimageSpecificationReturnsNoImages", 256 },
    { "UnableToAddOrRemoveProfile", 257 },
    { "UnableToAverageImageSequence", 258 },
    { "UnableToBlurImage", 259 },
    { "UnableToChopImage", 260 },
    { "UnableToColorMatrixImage", 261 },
    { "UnableToConstituteImage", 262 },
    { "UnableToConvolveImage", 263 },
    { "UnableToEdgeImage", 264 },
    { "UnableToEqualizeImage", 265 },
    { "UnableToFilterImage", 266 },
    { "UnableToFormatImageMetadata", 267 },
    { "UnableToFrameImage", 268 },
    { "UnableToOilPaintImage", 269 },
    { "UnableToPaintImage", 270 },
    { "UnableToRaiseImage", 271 },
    { "UnableToSharpenImage", 272 },
    { "UnableToThresholdImage", 273 },
    { "UnableToWaveImage", 274 },
    { "UnrecognizedAttribute", 275 },
    { "UnrecognizedChannelType", 276 },
    { "UnrecognizedColor", 277 },
    { "UnrecognizedColormapType", 278 },
    { "UnrecognizedColorspace", 279 },
    { "UnrecognizedCommand", 280 },
    { "UnrecognizedComposeOperator", 281 },
    { "UnrecognizedDisposeMethod", 282 },
    { "UnrecognizedElement", 283 },
    { "UnrecognizedEndianType", 284 },
    { "UnrecognizedGravityType", 285 },
    { "UnrecognizedHighlightStyle", 286 },
    { "UnrecognizedImageCompression", 287 },
    { "UnrecognizedImageFilter", 288 },
    { "UnrecognizedImageFormat", 289 },
    { "UnrecognizedImageMode", 290 },
    { "UnrecognizedImageType", 291 },
    { "UnrecognizedIntentType", 292 },
    { "UnrecognizedInterlaceType", 293 },
    { "UnrecognizedListType", 294 },
    { "UnrecognizedMetric", 295 },
    { "UnrecognizedModeType", 296 },
    { "UnrecognizedNoiseType", 297 },
    { "UnrecognizedOperator", 298 },
    { "UnrecognizedOption", 299 },
    { "UnrecognizedPerlMagickMethod", 300 },
    { "UnrecognizedPixelMap", 301 },
    { "UnrecognizedPreviewType", 302 },
    { "UnrecognizedResourceType", 303 },
    { "UnrecognizedType", 304 },
    { "UnrecognizedUnitsType", 305 },
    { "UnrecognizedVirtualPixelMethod", 306 },
    { "UnsupportedSamplingFactor", 307 },
    { "UsageError", 308 },
    { "InvalidColorspaceType", 309 },
    { "InvalidEndianType", 310 },
    { "InvalidImageType", 311 },
    { "InvalidInterlaceType", 312 },
    { "MissingAnImageFilename", 313 },
    { "MissingArgument", 314 },
    { "NoImagesWereLoaded", 315 },
    { "OptionLengthExceedsLimit", 316 },
    { "RequestDidNotReturnAnImage", 317 },
    { "UnableToOpenXServer", 318 },
    { "UnableToPersistKey", 319 },
    { "UnrecognizedColormapType", 320 },
    { "UnrecognizedColorspaceType", 321 },
    { "UnrecognizedDisposeMethod", 322 },
    { "UnrecognizedEndianType", 323 },
    { "UnrecognizedFilterType", 324 },
    { "UnrecognizedImageCompressionType", 325 },
    { "UnrecognizedImageType", 326 },
    { "UnrecognizedInterlaceType", 327 },
    { "UnrecognizedOption", 328 },
    { "UnrecognizedResourceType", 329 },
    { "UnrecognizedVirtualPixelMethod", 330 },
    { "UnrecognizedColor", 331 },
    { "ImageExpected", 332 },
    { "ImageInfoExpected", 333 },
    { "StructureSizeMismatch", 334 },
    { "UnableToGetRegistryID", 335 },
    { "UnableToLocateImage", 336 },
    { "UnableToSetRegistry", 337 },
    { "Default", 338 },
    { "Default", 339 },
    { "CacheResourcesExhausted", 340 },
    { "ImagePixelLimitExceeded", 341 },
    { "MemoryAllocationFailed", 342 },
    { "NoPixelsDefinedInCache", 343 },
    { "PixelCacheAllocationFailed", 344 },
    { "UnableToAddColorProfile", 345 },
    { "UnableToAddGenericProfile", 346 },
    { "UnableToAddIPTCProfile", 347 },
    { "UnableToAllocateCoefficients", 348 },
    { "UnableToAllocateColormap", 349 },
    { "UnableToAllocateICCProfile", 350 },
    { "UnableToAllocateString", 351 },
    { "UnableToAnnotateImage", 352 },
    { "UnableToAverageImageSequence", 353 },
    { "UnableToCloneDrawingWand", 354 },
    { "UnableToCloneImage", 355 },
    { "UnableToComputeImageSignature", 356 },
    { "UnableToConstituteImage", 357 },
    { "UnableToConvertFont", 358 },
    { "UnableToConvertStringToTokens", 359 },
    { "UnableToCreateColormap", 360 },
    { "UnableToCreateColorTransform", 361 },
    { "UnableToCreateCommandWidget", 362 },
    { "UnableToCreateImageGroup", 363 },
    { "UnableToCreateImageMontage", 364 },
    { "UnableToCreateXWindow", 365 },
    { "UnableToCropImage", 366 },
    { "UnableToDespeckleImage", 367 },
    { "UnableToDetermineImageClass", 368 },
    { "UnableToDetermineTheNumberOfImageColors", 369 },
    { "UnableToDitherImage", 370 },
    { "UnableToDrawOnImage", 371 },
    { "UnableToEdgeImage", 372 },
    { "UnableToEmbossImage", 373 },
    { "UnableToEnhanceImage", 374 },
    { "UnableToFloodfillImage", 375 },
    { "UnableToGammaCorrectImage", 376 },
    { "UnableToGetBestIconSize", 377 },
    { "UnableToGetFromRegistry", 378 },
    { "UnableToGetPackageInfo", 379 },
    { "UnableToLevelImage", 380 },
    { "UnableToMagnifyImage", 381 },
    { "UnableToManageColor", 382 },
    { "UnableToMapImage", 383 },
    { "UnableToMapImageSequence", 384 },
    { "UnableToMedianFilterImage", 385 },
    { "UnableToMotionBlurImage", 386 },
    { "UnableToNoiseFilterImage", 387 },
    { "UnableToNormalizeImage", 388 },
    { "UnableToOpenColorProfile", 389 },
    { "UnableToQuantizeImage", 390 },
    { "UnableToQuantizeImageSequence", 391 },
    { "UnableToReadTextChunk", 392 },
    { "UnableToReadXImage", 393 },
    { "UnableToReadXServerColormap", 394 },
    { "UnableToResizeImage", 395 },
    { "UnableToRotateImage", 396 },
    { "UnableToSampleImage", 397 },
    { "UnableToScaleImage", 398 },
    { "UnableToSelectImage", 399 },
    { "UnableToSharpenImage", 400 },
    { "UnableToShaveImage", 401 },
    { "UnableToShearImage", 402 },
    { "UnableToSortImageColormap", 403 },
    { "UnableToThresholdImage", 404 },
    { "UnableToTransformColorspace", 405 },
    { "MemoryAllocationFailed", 406 },
    { "UnableToAllocateAscii85Info", 407 },
    { "UnableToAllocateCacheInfo", 408 },
    { "UnableToAllocateCacheView", 409 },
    { "UnableToAllocateColorInfo", 410 },
    { "UnableToAllocateDashPattern", 411 },
    { "UnableToAllocateDelegateInfo", 412 },
    { "UnableToAllocateDerivatives", 413 },
    { "UnableToAllocateDrawContext", 414 },
    { "UnableToAllocateDrawInfo", 415 },
    { "UnableToAllocateDrawingWand", 416 },
    { "UnableToAllocateGammaMap", 417 },
    { "UnableToAllocateImage", 418 },
    { "UnableToAllocateImagePixels", 419 },
    { "UnableToAllocateLogInfo", 420 },
    { "UnableToAllocateMagicInfo", 421 },
    { "UnableToAllocateMagickInfo", 422 },
    { "UnableToAllocateModuleInfo", 423 },
    { "UnableToAllocateMontageInfo", 424 },
    { "UnableToAllocateQuantizeInfo", 425 },
    { "UnableToAllocateRandomKernel", 426 },
    { "UnableToAllocateRegistryInfo", 427 },
    { "UnableToAllocateSemaphoreInfo", 428 },
    { "UnableToAllocateString", 429 },
    { "UnableToAllocateTypeInfo", 430 },
    { "UnableToAllocateWand", 431 },
    { "UnableToAnimateImageSequence", 432 },
    { "UnableToCloneBlobInfo", 433 },
    { "UnableToCloneCacheInfo", 434 },
    { "UnableToCloneImage", 435 },
    { "UnableToCloneImageInfo", 436 },
    { "UnableToConcatenateString", 437 },
    { "UnableToConvertText", 438 },
    { "UnableToCreateColormap", 439 },
    { "UnableToDisplayImage", 440 },
    { "UnableToEscapeString", 441 },
    { "UnableToInterpretMSLImage", 442 },
    { "MemoryAllocationFailed", 443 },
    { "ImageDoesNotContainTheStreamGeometry", 444 },
    { "NoStreamHandlerIsDefined", 445 },
    { "PixelCacheIsNotOpen", 446 },
    { "UnableToAcquirePixelStream", 447 },
    { "UnableToSetPixelStream", 448 },
    { "UnableToSyncPixelStream", 449 },
    { "Default", 450 },
    { "Default", 451 },
    { "FontSubstitutionRequired", 452 },
    { "UnableToGetTypeMetrics", 453 },
    { "UnableToInitializeFreetypeLibrary", 454 },
    { "UnableToReadFont", 455 },
    { "UnrecognizedFontEncoding", 456 },
    { "Default", 457 },
    { "Default", 458 },
    { "InvalidColormapIndex", 459 },
    { "WandAPINotImplemented", 460 },
    { "WandContainsNoImageIndexs", 461 },
    { "WandContainsNoImages", 462 },
    { "ColorIsNotKnownToServer", 463 },
    { "NoWindowWithSpecifiedIDExists", 464 },
    { "StandardColormapIsNotInitialized", 465 },
    { "UnableToConnectToRemoteDisplay", 466 },
    { "UnableToCreateBitmap", 467 },
    { "UnableToCreateColormap", 468 },
    { "UnableToCreatePixmap", 469 },
    { "UnableToCreateProperty", 470 },
    { "UnableToCreateStandardColormap", 471 },
    { "UnableToDisplayImageInfo", 472 },
    { "UnableToGetProperty", 473 },
    { "UnableToGetStandardColormap", 474 },
    { "UnableToGetVisual", 475 },
    { "UnableToGrabMouse", 476 },
    { "UnableToLoadFont", 477 },
    { "UnableToMatchVisualToStandardColormap", 478 },
    { "UnableToOpenXServer", 479 },
    { "UnableToReadXAttributes", 480 },
    { "UnableToReadXWindowImage", 481 },
    { "UnrecognizedColormapType", 482 },
    { "UnrecognizedGravityType", 483 },
    { "UnrecognizedVisualSpecifier", 484 },
    { "UnableToAllocateXHints", 485 },
    { "UnableToCreateCursor", 486 },
    { "UnableToCreateGraphicContext", 487 },
    { "UnableToCreateStandardColormap", 488 },
    { "UnableToCreateTextProperty", 489 },
    { "UnableToCreateXImage", 490 },
    { "UnableToCreateXPixmap", 491 },
    { "UnableToCreateXWindow", 492 },
    { "UnableToDisplayImage", 493 },
    { "UnableToDitherImage", 494 },
    { "UnableToGetPixelInfo", 495 },
    { "UnableToGetVisual", 496 },
    { "UnableToLoadFont", 497 },
    { "UnableToMakeXWindow", 498 },
    { "UnableToOpenXServer", 499 },
    { "UnableToViewFonts", 500 },
    { "UnableToGetVisual", 501 },
    { "UsingDefaultVisual", 502 },
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
    "Failed to allocate argument list.",
    "Failed to allocate Ghostscript interpreter.",
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
    "hald clut image dimensions are invalid",
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
    "unable to clone drawing wand",
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
