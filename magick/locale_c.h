#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 517

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
#define MGK_ConfigureErrorStringTokenLengthExceeded 91
#define MGK_ConfigureErrorUnableToAccessConfigureFile 92
#define MGK_ConfigureErrorUnableToAccessFontFile 93
#define MGK_ConfigureErrorUnableToAccessLogFile 94
#define MGK_ConfigureErrorUnableToAccessModuleFile 95
#define MGK_ConfigureFatalErrorDefault 96
#define MGK_ConfigureWarningDefault 97
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 98
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 99
#define MGK_CorruptImageErrorColormapExceeds256Colors 100
#define MGK_CorruptImageErrorCorruptImage 101
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 102
#define MGK_CorruptImageErrorImageTypeNotSupported 103
#define MGK_CorruptImageErrorImproperImageHeader 104
#define MGK_CorruptImageErrorInsufficientImageDataInFile 105
#define MGK_CorruptImageErrorInvalidColormapIndex 106
#define MGK_CorruptImageErrorInvalidFileFormatVersion 107
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 108
#define MGK_CorruptImageErrorMissingImageChannel 109
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 110
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 111
#define MGK_CorruptImageErrorNotEnoughTiles 112
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 113
#define MGK_CorruptImageErrorSubsamplingRequiresEvenWidth 114
#define MGK_CorruptImageErrorTooMuchImageDataInFile 115
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 116
#define MGK_CorruptImageErrorUnableToReadColorProfile 117
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 118
#define MGK_CorruptImageErrorUnableToReadGenericProfile 119
#define MGK_CorruptImageErrorUnableToReadImageData 120
#define MGK_CorruptImageErrorUnableToReadImageHeader 121
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 122
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 123
#define MGK_CorruptImageErrorUnableToReadSubImageData 124
#define MGK_CorruptImageErrorUnableToReadVIDImage 125
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 126
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 127
#define MGK_CorruptImageErrorUnableToUncompressImage 128
#define MGK_CorruptImageErrorUnexpectedEndOfFile 129
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 130
#define MGK_CorruptImageErrorUnknownPatternType 131
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 132
#define MGK_CorruptImageErrorUnrecognizedImageCompression 133
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 134
#define MGK_CorruptImageFatalErrorUnableToPersistKey 135
#define MGK_CorruptImageWarningCompressionNotValid 136
#define MGK_CorruptImageWarningImproperImageHeader 137
#define MGK_CorruptImageWarningLengthAndFilesizeDoNotMatch 138
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 139
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 140
#define MGK_CorruptImageWarningSkipToSyncByte 141
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 142
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 143
#define MGK_CorruptImageWarningUnrecognizedImageCompression 144
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 145
#define MGK_DelegateErrorDelegateFailed 146
#define MGK_DelegateErrorFailedToAllocateArgumentList 147
#define MGK_DelegateErrorFailedToAllocateGhostscriptInterpreter 148
#define MGK_DelegateErrorFailedToComputeOutputSize 149
#define MGK_DelegateErrorFailedToRenderFile 150
#define MGK_DelegateErrorFailedToScanFile 151
#define MGK_DelegateErrorNoTagFound 152
#define MGK_DelegateErrorPostscriptDelegateFailed 153
#define MGK_DelegateErrorUnableToCreateImage 154
#define MGK_DelegateErrorUnableToCreateImageComponent 155
#define MGK_DelegateErrorUnableToDecodeImageFile 156
#define MGK_DelegateErrorUnableToEncodeImageFile 157
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 158
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 159
#define MGK_DelegateErrorUnableToManageJP2Stream 160
#define MGK_DelegateErrorUnableToWriteSVGFormat 161
#define MGK_DelegateFatalErrorDefault 162
#define MGK_DelegateWarningDefault 163
#define MGK_DrawErrorAlreadyPushingPatternDefinition 164
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 165
#define MGK_DrawErrorUnableToPrint 166
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 167
#define MGK_DrawFatalErrorDefault 168
#define MGK_DrawWarningNotARelativeURL 169
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 170
#define MGK_DrawWarningURLNotFound 171
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 172
#define MGK_FileOpenErrorUnableToOpenFile 173
#define MGK_FileOpenErrorUnableToWriteFile 174
#define MGK_FileOpenFatalErrorDefault 175
#define MGK_FileOpenWarningDefault 176
#define MGK_ImageErrorAngleIsDiscontinuous 177
#define MGK_ImageErrorColorspaceColorProfileMismatch 178
#define MGK_ImageErrorImageColorspaceDiffers 179
#define MGK_ImageErrorImageColorspaceMismatch 180
#define MGK_ImageErrorImageDifferenceExceedsLimit 181
#define MGK_ImageErrorImageDoesNotContainResolution 182
#define MGK_ImageErrorImageOpacityDiffers 183
#define MGK_ImageErrorImageSequenceIsRequired 184
#define MGK_ImageErrorImageSizeDiffers 185
#define MGK_ImageErrorInvalidColormapIndex 186
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 187
#define MGK_ImageErrorNoImagesWereFound 188
#define MGK_ImageErrorNoImagesWereLoaded 189
#define MGK_ImageErrorNoLocaleImageAttribute 190
#define MGK_ImageErrorTooManyClusters 191
#define MGK_ImageErrorUnableToAppendImage 192
#define MGK_ImageErrorUnableToAssignProfile 193
#define MGK_ImageErrorUnableToAverageImage 194
#define MGK_ImageErrorUnableToCoalesceImage 195
#define MGK_ImageErrorUnableToCompareImages 196
#define MGK_ImageErrorUnableToCreateImageMosaic 197
#define MGK_ImageErrorUnableToCreateStereoImage 198
#define MGK_ImageErrorUnableToDeconstructImageSequence 199
#define MGK_ImageErrorUnableToFlattenImage 200
#define MGK_ImageErrorUnableToGetClipMask 201
#define MGK_ImageErrorUnableToHandleImageChannel 202
#define MGK_ImageErrorUnableToResizeImage 203
#define MGK_ImageErrorUnableToSegmentImage 204
#define MGK_ImageErrorUnableToSetClipMask 205
#define MGK_ImageErrorUnableToShearImage 206
#define MGK_ImageErrorWidthOrHeightExceedsLimit 207
#define MGK_ImageFatalErrorUnableToPersistKey 208
#define MGK_ImageWarningDefault 209
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 210
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 211
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 212
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 213
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 214
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 215
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 216
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 217
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 218
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 219
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 220
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 221
#define MGK_MissingDelegateFatalErrorDefault 222
#define MGK_MissingDelegateWarningDefault 223
#define MGK_ModuleErrorFailedToCloseModule 224
#define MGK_ModuleErrorFailedToFindSymbol 225
#define MGK_ModuleErrorUnableToLoadModule 226
#define MGK_ModuleErrorUnableToRegisterImageFormat 227
#define MGK_ModuleErrorUnrecognizedModule 228
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 229
#define MGK_ModuleWarningDefault 230
#define MGK_MonitorErrorDefault 231
#define MGK_MonitorFatalErrorDefault 232
#define MGK_MonitorFatalErrorUserRequestedTerminationBySignal 233
#define MGK_MonitorWarningDefault 234
#define MGK_OptionErrorBevelWidthIsNegative 235
#define MGK_OptionErrorColorSeparatedImageRequired 236
#define MGK_OptionErrorFrameIsLessThanImageSize 237
#define MGK_OptionErrorGeometryDimensionsAreZero 238
#define MGK_OptionErrorGeometryDoesNotContainImage 239
#define MGK_OptionErrorHaldClutImageDimensionsInvalid 240
#define MGK_OptionErrorImagesAreNotTheSameSize 241
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 242
#define MGK_OptionErrorImageSmallerThanKernelWidth 243
#define MGK_OptionErrorImageSmallerThanRadius 244
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 245
#define MGK_OptionErrorInputImagesAlreadySpecified 246
#define MGK_OptionErrorInvalidSubimageSpecification 247
#define MGK_OptionErrorKernelRadiusIsTooSmall 248
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 249
#define MGK_OptionErrorMatrixIsNotSquare 250
#define MGK_OptionErrorMatrixOrderOutOfRange 251
#define MGK_OptionErrorMissingAnImageFilename 252
#define MGK_OptionErrorMissingArgument 253
#define MGK_OptionErrorMustSpecifyAnImageName 254
#define MGK_OptionErrorMustSpecifyImageSize 255
#define MGK_OptionErrorNoBlobDefined 256
#define MGK_OptionErrorNoImagesDefined 257
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 258
#define MGK_OptionErrorNoProfileNameWasGiven 259
#define MGK_OptionErrorNullBlobArgument 260
#define MGK_OptionErrorReferenceImageRequired 261
#define MGK_OptionErrorReferenceIsNotMyType 262
#define MGK_OptionErrorRequestDidNotReturnAnImage 263
#define MGK_OptionErrorSteganoImageRequired 264
#define MGK_OptionErrorStereoImageRequired 265
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 266
#define MGK_OptionErrorUnableToAddOrRemoveProfile 267
#define MGK_OptionErrorUnableToAverageImageSequence 268
#define MGK_OptionErrorUnableToBlurImage 269
#define MGK_OptionErrorUnableToChopImage 270
#define MGK_OptionErrorUnableToColorMatrixImage 271
#define MGK_OptionErrorUnableToConstituteImage 272
#define MGK_OptionErrorUnableToConvolveImage 273
#define MGK_OptionErrorUnableToEdgeImage 274
#define MGK_OptionErrorUnableToEqualizeImage 275
#define MGK_OptionErrorUnableToFilterImage 276
#define MGK_OptionErrorUnableToFormatImageMetadata 277
#define MGK_OptionErrorUnableToFrameImage 278
#define MGK_OptionErrorUnableToOilPaintImage 279
#define MGK_OptionErrorUnableToPaintImage 280
#define MGK_OptionErrorUnableToRaiseImage 281
#define MGK_OptionErrorUnableToSharpenImage 282
#define MGK_OptionErrorUnableToThresholdImage 283
#define MGK_OptionErrorUnableToWaveImage 284
#define MGK_OptionErrorUnrecognizedAttribute 285
#define MGK_OptionErrorUnrecognizedChannelType 286
#define MGK_OptionErrorUnrecognizedColor 287
#define MGK_OptionErrorUnrecognizedColormapType 288
#define MGK_OptionErrorUnrecognizedColorspace 289
#define MGK_OptionErrorUnrecognizedCommand 290
#define MGK_OptionErrorUnrecognizedComposeOperator 291
#define MGK_OptionErrorUnrecognizedDisposeMethod 292
#define MGK_OptionErrorUnrecognizedElement 293
#define MGK_OptionErrorUnrecognizedEndianType 294
#define MGK_OptionErrorUnrecognizedGravityType 295
#define MGK_OptionErrorUnrecognizedHighlightStyle 296
#define MGK_OptionErrorUnrecognizedImageCompression 297
#define MGK_OptionErrorUnrecognizedImageFilter 298
#define MGK_OptionErrorUnrecognizedImageFormat 299
#define MGK_OptionErrorUnrecognizedImageMode 300
#define MGK_OptionErrorUnrecognizedImageType 301
#define MGK_OptionErrorUnrecognizedIntentType 302
#define MGK_OptionErrorUnrecognizedInterlaceType 303
#define MGK_OptionErrorUnrecognizedListType 304
#define MGK_OptionErrorUnrecognizedMetric 305
#define MGK_OptionErrorUnrecognizedModeType 306
#define MGK_OptionErrorUnrecognizedNoiseType 307
#define MGK_OptionErrorUnrecognizedOperator 308
#define MGK_OptionErrorUnrecognizedOption 309
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 310
#define MGK_OptionErrorUnrecognizedPixelMap 311
#define MGK_OptionErrorUnrecognizedPreviewType 312
#define MGK_OptionErrorUnrecognizedResourceType 313
#define MGK_OptionErrorUnrecognizedType 314
#define MGK_OptionErrorUnrecognizedUnitsType 315
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 316
#define MGK_OptionErrorUnsupportedSamplingFactor 317
#define MGK_OptionErrorUsageError 318
#define MGK_OptionFatalErrorInvalidColorspaceType 319
#define MGK_OptionFatalErrorInvalidEndianType 320
#define MGK_OptionFatalErrorInvalidImageType 321
#define MGK_OptionFatalErrorInvalidInterlaceType 322
#define MGK_OptionFatalErrorMissingAnImageFilename 323
#define MGK_OptionFatalErrorMissingArgument 324
#define MGK_OptionFatalErrorNoImagesWereLoaded 325
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 326
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 327
#define MGK_OptionFatalErrorUnableToOpenXServer 328
#define MGK_OptionFatalErrorUnableToPersistKey 329
#define MGK_OptionFatalErrorUnrecognizedColormapType 330
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 331
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 332
#define MGK_OptionFatalErrorUnrecognizedEndianType 333
#define MGK_OptionFatalErrorUnrecognizedFilterType 334
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 335
#define MGK_OptionFatalErrorUnrecognizedImageType 336
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 337
#define MGK_OptionFatalErrorUnrecognizedOption 338
#define MGK_OptionFatalErrorUnrecognizedResourceType 339
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 340
#define MGK_OptionWarningUnrecognizedColor 341
#define MGK_RegistryErrorImageExpected 342
#define MGK_RegistryErrorImageInfoExpected 343
#define MGK_RegistryErrorStructureSizeMismatch 344
#define MGK_RegistryErrorUnableToGetRegistryID 345
#define MGK_RegistryErrorUnableToLocateImage 346
#define MGK_RegistryErrorUnableToSetRegistry 347
#define MGK_RegistryFatalErrorDefault 348
#define MGK_RegistryWarningDefault 349
#define MGK_ResourceLimitErrorCacheResourcesExhausted 350
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 351
#define MGK_ResourceLimitErrorMemoryAllocationFailed 352
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 353
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 354
#define MGK_ResourceLimitErrorUnableToAddColorProfile 355
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 356
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 357
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 358
#define MGK_ResourceLimitErrorUnableToAllocateColormap 359
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 360
#define MGK_ResourceLimitErrorUnableToAllocateString 361
#define MGK_ResourceLimitErrorUnableToAnnotateImage 362
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 363
#define MGK_ResourceLimitErrorUnableToCloneDrawingWand 364
#define MGK_ResourceLimitErrorUnableToCloneImage 365
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 366
#define MGK_ResourceLimitErrorUnableToConstituteImage 367
#define MGK_ResourceLimitErrorUnableToConvertFont 368
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 369
#define MGK_ResourceLimitErrorUnableToCreateColormap 370
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 371
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 372
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 373
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 374
#define MGK_ResourceLimitErrorUnableToCreateXWindow 375
#define MGK_ResourceLimitErrorUnableToCropImage 376
#define MGK_ResourceLimitErrorUnableToDespeckleImage 377
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 378
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 379
#define MGK_ResourceLimitErrorUnableToDitherImage 380
#define MGK_ResourceLimitErrorUnableToDrawOnImage 381
#define MGK_ResourceLimitErrorUnableToEdgeImage 382
#define MGK_ResourceLimitErrorUnableToEmbossImage 383
#define MGK_ResourceLimitErrorUnableToEnhanceImage 384
#define MGK_ResourceLimitErrorUnableToFloodfillImage 385
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 386
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 387
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 388
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 389
#define MGK_ResourceLimitErrorUnableToLevelImage 390
#define MGK_ResourceLimitErrorUnableToMagnifyImage 391
#define MGK_ResourceLimitErrorUnableToManageColor 392
#define MGK_ResourceLimitErrorUnableToMapImage 393
#define MGK_ResourceLimitErrorUnableToMapImageSequence 394
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 395
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 396
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 397
#define MGK_ResourceLimitErrorUnableToNormalizeImage 398
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 399
#define MGK_ResourceLimitErrorUnableToQuantizeImage 400
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 401
#define MGK_ResourceLimitErrorUnableToReadTextChunk 402
#define MGK_ResourceLimitErrorUnableToReadXImage 403
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 404
#define MGK_ResourceLimitErrorUnableToResizeImage 405
#define MGK_ResourceLimitErrorUnableToRotateImage 406
#define MGK_ResourceLimitErrorUnableToSampleImage 407
#define MGK_ResourceLimitErrorUnableToScaleImage 408
#define MGK_ResourceLimitErrorUnableToSelectImage 409
#define MGK_ResourceLimitErrorUnableToSharpenImage 410
#define MGK_ResourceLimitErrorUnableToShaveImage 411
#define MGK_ResourceLimitErrorUnableToShearImage 412
#define MGK_ResourceLimitErrorUnableToSortImageColormap 413
#define MGK_ResourceLimitErrorUnableToThresholdImage 414
#define MGK_ResourceLimitErrorUnableToTransformColorspace 415
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 416
#define MGK_ResourceLimitFatalErrorSemaporeOperationFailed 417
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 418
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 419
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 420
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 421
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 422
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 423
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 424
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 425
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 426
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 427
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 428
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 429
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 430
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 431
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 432
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 433
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 434
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 435
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 436
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 437
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 438
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 439
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 440
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 441
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 442
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 443
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 444
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 445
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 446
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 447
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 448
#define MGK_ResourceLimitFatalErrorUnableToConvertText 449
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 450
#define MGK_ResourceLimitFatalErrorUnableToDestroySemaphore 451
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 452
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 453
#define MGK_ResourceLimitFatalErrorUnableToInitializeSemaphore 454
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 455
#define MGK_ResourceLimitFatalErrorUnableToLockSemaphore 456
#define MGK_ResourceLimitFatalErrorUnableToUnlockSemaphore 457
#define MGK_ResourceLimitWarningMemoryAllocationFailed 458
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 459
#define MGK_StreamErrorNoStreamHandlerIsDefined 460
#define MGK_StreamErrorPixelCacheIsNotOpen 461
#define MGK_StreamErrorUnableToAcquirePixelStream 462
#define MGK_StreamErrorUnableToSetPixelStream 463
#define MGK_StreamErrorUnableToSyncPixelStream 464
#define MGK_StreamFatalErrorDefault 465
#define MGK_StreamWarningDefault 466
#define MGK_TypeErrorFontSubstitutionRequired 467
#define MGK_TypeErrorUnableToGetTypeMetrics 468
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 469
#define MGK_TypeErrorUnableToReadFont 470
#define MGK_TypeErrorUnrecognizedFontEncoding 471
#define MGK_TypeFatalErrorDefault 472
#define MGK_TypeWarningDefault 473
#define MGK_WandErrorInvalidColormapIndex 474
#define MGK_WandErrorWandAPINotImplemented 475
#define MGK_WandErrorWandContainsNoImageIndexs 476
#define MGK_WandErrorWandContainsNoImages 477
#define MGK_XServerErrorColorIsNotKnownToServer 478
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 479
#define MGK_XServerErrorStandardColormapIsNotInitialized 480
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 481
#define MGK_XServerErrorUnableToCreateBitmap 482
#define MGK_XServerErrorUnableToCreateColormap 483
#define MGK_XServerErrorUnableToCreatePixmap 484
#define MGK_XServerErrorUnableToCreateProperty 485
#define MGK_XServerErrorUnableToCreateStandardColormap 486
#define MGK_XServerErrorUnableToDisplayImageInfo 487
#define MGK_XServerErrorUnableToGetProperty 488
#define MGK_XServerErrorUnableToGetStandardColormap 489
#define MGK_XServerErrorUnableToGetVisual 490
#define MGK_XServerErrorUnableToGrabMouse 491
#define MGK_XServerErrorUnableToLoadFont 492
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 493
#define MGK_XServerErrorUnableToOpenXServer 494
#define MGK_XServerErrorUnableToReadXAttributes 495
#define MGK_XServerErrorUnableToReadXWindowImage 496
#define MGK_XServerErrorUnrecognizedColormapType 497
#define MGK_XServerErrorUnrecognizedGravityType 498
#define MGK_XServerErrorUnrecognizedVisualSpecifier 499
#define MGK_XServerFatalErrorUnableToAllocateXHints 500
#define MGK_XServerFatalErrorUnableToCreateCursor 501
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 502
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 503
#define MGK_XServerFatalErrorUnableToCreateTextProperty 504
#define MGK_XServerFatalErrorUnableToCreateXImage 505
#define MGK_XServerFatalErrorUnableToCreateXPixmap 506
#define MGK_XServerFatalErrorUnableToCreateXWindow 507
#define MGK_XServerFatalErrorUnableToDisplayImage 508
#define MGK_XServerFatalErrorUnableToDitherImage 509
#define MGK_XServerFatalErrorUnableToGetPixelInfo 510
#define MGK_XServerFatalErrorUnableToGetVisual 511
#define MGK_XServerFatalErrorUnableToLoadFont 512
#define MGK_XServerFatalErrorUnableToMakeXWindow 513
#define MGK_XServerFatalErrorUnableToOpenXServer 514
#define MGK_XServerFatalErrorUnableToViewFonts 515
#define MGK_XServerWarningUnableToGetVisual 516
#define MGK_XServerWarningUsingDefaultVisual 517

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
    { "Monitor", 33 },
    { "Option", 36 },
    { "Registry", 39 },
    { "Resource/Limit", 42 },
    { "Stream", 45 },
    { "Type", 48 },
    { "Wand", 51 },
    { "XServer", 52 },
    { 0, 54 }
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
    { "Configure/FatalError", 95, ConfigureFatalError },
    { "Configure/Warning", 96, ConfigureWarning },
    { "Corrupt/Image/Error", 97, CorruptImageError },
    { "Corrupt/Image/FatalError", 134, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 135, CorruptImageWarning },
    { "Delegate/Error", 145, DelegateError },
    { "Delegate/FatalError", 161, DelegateFatalError },
    { "Delegate/Warning", 162, DelegateWarning },
    { "Draw/Error", 163, DrawError },
    { "Draw/FatalError", 167, DrawFatalError },
    { "Draw/Warning", 168, DrawWarning },
    { "File/Open/Error", 171, FileOpenError },
    { "File/Open/FatalError", 174, FileOpenFatalError },
    { "File/Open/Warning", 175, FileOpenWarning },
    { "Image/Error", 176, ImageError },
    { "Image/FatalError", 207, ImageFatalError },
    { "Image/Warning", 208, ImageWarning },
    { "Missing/Delegate/Error", 209, MissingDelegateError },
    { "Missing/Delegate/FatalError", 221, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 222, MissingDelegateWarning },
    { "Module/Error", 223, ModuleError },
    { "Module/FatalError", 228, ModuleFatalError },
    { "Module/Warning", 229, ModuleWarning },
    { "Monitor/Error", 230, MonitorError },
    { "Monitor/FatalError", 231, MonitorFatalError },
    { "Monitor/Warning", 233, MonitorWarning },
    { "Option/Error", 234, OptionError },
    { "Option/FatalError", 318, OptionFatalError },
    { "Option/Warning", 340, OptionWarning },
    { "Registry/Error", 341, RegistryError },
    { "Registry/FatalError", 347, RegistryFatalError },
    { "Registry/Warning", 348, RegistryWarning },
    { "Resource/Limit/Error", 349, ResourceLimitError },
    { "Resource/Limit/FatalError", 415, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 457, ResourceLimitWarning },
    { "Stream/Error", 458, StreamError },
    { "Stream/FatalError", 464, StreamFatalError },
    { "Stream/Warning", 465, StreamWarning },
    { "Type/Error", 466, TypeError },
    { "Type/FatalError", 471, TypeFatalError },
    { "Type/Warning", 472, TypeWarning },
    { "Wand/Error", 473, WandError },
    { "XServer/Error", 477, XServerError },
    { "XServer/FatalError", 499, XServerFatalError },
    { "XServer/Warning", 515, XServerWarning },
    { 0, 517, UndefinedException }
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
    { "StringTokenLengthExceeded", 91 },
    { "UnableToAccessConfigureFile", 92 },
    { "UnableToAccessFontFile", 93 },
    { "UnableToAccessLogFile", 94 },
    { "UnableToAccessModuleFile", 95 },
    { "Default", 96 },
    { "Default", 97 },
    { "AnErrorHasOccurredReadingFromFile", 98 },
    { "AnErrorHasOccurredWritingToFile", 99 },
    { "ColormapExceeds256Colors", 100 },
    { "CorruptImage", 101 },
    { "ImageFileDoesNotContainAnyImageData", 102 },
    { "ImageTypeNotSupported", 103 },
    { "ImproperImageHeader", 104 },
    { "InsufficientImageDataInFile", 105 },
    { "InvalidColormapIndex", 106 },
    { "InvalidFileFormatVersion", 107 },
    { "LengthAndFilesizeDoNotMatch", 108 },
    { "MissingImageChannel", 109 },
    { "NegativeOrZeroImageSize", 110 },
    { "NonOS2HeaderSizeError", 111 },
    { "NotEnoughTiles", 112 },
    { "StaticPlanesValueNotEqualToOne", 113 },
    { "SubsamplingRequiresEvenWidth", 114 },
    { "TooMuchImageDataInFile", 115 },
    { "UnableToReadColormapFromDumpFile", 116 },
    { "UnableToReadColorProfile", 117 },
    { "UnableToReadExtensionBlock", 118 },
    { "UnableToReadGenericProfile", 119 },
    { "UnableToReadImageData", 120 },
    { "UnableToReadImageHeader", 121 },
    { "UnableToReadIPTCProfile", 122 },
    { "UnableToReadPixmapFromDumpFile", 123 },
    { "UnableToReadSubImageData", 124 },
    { "UnableToReadVIDImage", 125 },
    { "UnableToReadWindowNameFromDumpFile", 126 },
    { "UnableToRunlengthDecodeImage", 127 },
    { "UnableToUncompressImage", 128 },
    { "UnexpectedEndOfFile", 129 },
    { "UnexpectedSamplingFactor", 130 },
    { "UnknownPatternType", 131 },
    { "UnrecognizedBitsPerPixel", 132 },
    { "UnrecognizedImageCompression", 133 },
    { "UnrecognizedXWDHeader", 134 },
    { "UnableToPersistKey", 135 },
    { "CompressionNotValid", 136 },
    { "ImproperImageHeader", 137 },
    { "LengthAndFilesizeDoNotMatch", 138 },
    { "NegativeOrZeroImageSize", 139 },
    { "NonOS2HeaderSizeError", 140 },
    { "SkipToSyncByte", 141 },
    { "StaticPlanesValueNotEqualToOne", 142 },
    { "UnrecognizedBitsPerPixel", 143 },
    { "UnrecognizedImageCompression", 144 },
    { "UnrecognizedNumberOfColors", 145 },
    { "DelegateFailed", 146 },
    { "FailedToAllocateArgumentList", 147 },
    { "FailedToAllocateGhostscriptInterpreter", 148 },
    { "FailedToComputeOutputSize", 149 },
    { "FailedToRenderFile", 150 },
    { "FailedToScanFile", 151 },
    { "NoTagFound", 152 },
    { "PostscriptDelegateFailed", 153 },
    { "UnableToCreateImage", 154 },
    { "UnableToCreateImageComponent", 155 },
    { "UnableToDecodeImageFile", 156 },
    { "UnableToEncodeImageFile", 157 },
    { "UnableToInitializeFPXLibrary", 158 },
    { "UnableToInitializeWMFLibrary", 159 },
    { "UnableToManageJP2Stream", 160 },
    { "UnableToWriteSVGFormat", 161 },
    { "Default", 162 },
    { "Default", 163 },
    { "AlreadyPushingPatternDefinition", 164 },
    { "NonconformingDrawingPrimitiveDefinition", 165 },
    { "UnableToPrint", 166 },
    { "UnbalancedGraphicContextPushPop", 167 },
    { "Default", 168 },
    { "NotARelativeURL", 169 },
    { "NotCurrentlyPushingPatternDefinition", 170 },
    { "URLNotFound", 171 },
    { "UnableToCreateTemporaryFile", 172 },
    { "UnableToOpenFile", 173 },
    { "UnableToWriteFile", 174 },
    { "Default", 175 },
    { "Default", 176 },
    { "AngleIsDiscontinuous", 177 },
    { "ColorspaceColorProfileMismatch", 178 },
    { "ImageColorspaceDiffers", 179 },
    { "ImageColorspaceMismatch", 180 },
    { "ImageDifferenceExceedsLimit", 181 },
    { "ImageDoesNotContainResolution", 182 },
    { "ImageOpacityDiffers", 183 },
    { "ImageSequenceIsRequired", 184 },
    { "ImageSizeDiffers", 185 },
    { "InvalidColormapIndex", 186 },
    { "LeftAndRightImageSizesDiffer", 187 },
    { "NoImagesWereFound", 188 },
    { "NoImagesWereLoaded", 189 },
    { "NoLocaleImageAttribute", 190 },
    { "TooManyClusters", 191 },
    { "UnableToAppendImage", 192 },
    { "UnableToAssignProfile", 193 },
    { "UnableToAverageImage", 194 },
    { "UnableToCoalesceImage", 195 },
    { "UnableToCompareImages", 196 },
    { "UnableToCreateImageMosaic", 197 },
    { "UnableToCreateStereoImage", 198 },
    { "UnableToDeconstructImageSequence", 199 },
    { "UnableToFlattenImage", 200 },
    { "UnableToGetClipMask", 201 },
    { "UnableToHandleImageChannel", 202 },
    { "UnableToResizeImage", 203 },
    { "UnableToSegmentImage", 204 },
    { "UnableToSetClipMask", 205 },
    { "UnableToShearImage", 206 },
    { "WidthOrHeightExceedsLimit", 207 },
    { "UnableToPersistKey", 208 },
    { "Default", 209 },
    { "DPSLibraryIsNotAvailable", 210 },
    { "FPXLibraryIsNotAvailable", 211 },
    { "FreeTypeLibraryIsNotAvailable", 212 },
    { "JPEGLibraryIsNotAvailable", 213 },
    { "LCMSLibraryIsNotAvailable", 214 },
    { "LZWEncodingNotEnabled", 215 },
    { "NoDecodeDelegateForThisImageFormat", 216 },
    { "NoEncodeDelegateForThisImageFormat", 217 },
    { "TIFFLibraryIsNotAvailable", 218 },
    { "XMLLibraryIsNotAvailable", 219 },
    { "XWindowLibraryIsNotAvailable", 220 },
    { "ZipLibraryIsNotAvailable", 221 },
    { "Default", 222 },
    { "Default", 223 },
    { "FailedToCloseModule", 224 },
    { "FailedToFindSymbol", 225 },
    { "UnableToLoadModule", 226 },
    { "UnableToRegisterImageFormat", 227 },
    { "UnrecognizedModule", 228 },
    { "UnableToInitializeModuleLoader", 229 },
    { "Default", 230 },
    { "Default", 231 },
    { "Default", 232 },
    { "UserRequestedTerminationBySignal", 233 },
    { "Default", 234 },
    { "BevelWidthIsNegative", 235 },
    { "ColorSeparatedImageRequired", 236 },
    { "FrameIsLessThanImageSize", 237 },
    { "GeometryDimensionsAreZero", 238 },
    { "GeometryDoesNotContainImage", 239 },
    { "HaldClutImageDimensionsInvalid", 240 },
    { "ImagesAreNotTheSameSize", 241 },
    { "ImageSizeMustExceedBevelWidth", 242 },
    { "ImageSmallerThanKernelWidth", 243 },
    { "ImageSmallerThanRadius", 244 },
    { "ImageWidthsOrHeightsDiffer", 245 },
    { "InputImagesAlreadySpecified", 246 },
    { "InvalidSubimageSpecification", 247 },
    { "KernelRadiusIsTooSmall", 248 },
    { "KernelWidthMustBeAnOddNumber", 249 },
    { "MatrixIsNotSquare", 250 },
    { "MatrixOrderOutOfRange", 251 },
    { "MissingAnImageFilename", 252 },
    { "MissingArgument", 253 },
    { "MustSpecifyAnImageName", 254 },
    { "MustSpecifyImageSize", 255 },
    { "NoBlobDefined", 256 },
    { "NoImagesDefined", 257 },
    { "NonzeroWidthAndHeightRequired", 258 },
    { "NoProfileNameWasGiven", 259 },
    { "NullBlobArgument", 260 },
    { "ReferenceImageRequired", 261 },
    { "ReferenceIsNotMyType", 262 },
    { "RequestDidNotReturnAnImage", 263 },
    { "SteganoImageRequired", 264 },
    { "StereoImageRequired", 265 },
    { "SubimageSpecificationReturnsNoImages", 266 },
    { "UnableToAddOrRemoveProfile", 267 },
    { "UnableToAverageImageSequence", 268 },
    { "UnableToBlurImage", 269 },
    { "UnableToChopImage", 270 },
    { "UnableToColorMatrixImage", 271 },
    { "UnableToConstituteImage", 272 },
    { "UnableToConvolveImage", 273 },
    { "UnableToEdgeImage", 274 },
    { "UnableToEqualizeImage", 275 },
    { "UnableToFilterImage", 276 },
    { "UnableToFormatImageMetadata", 277 },
    { "UnableToFrameImage", 278 },
    { "UnableToOilPaintImage", 279 },
    { "UnableToPaintImage", 280 },
    { "UnableToRaiseImage", 281 },
    { "UnableToSharpenImage", 282 },
    { "UnableToThresholdImage", 283 },
    { "UnableToWaveImage", 284 },
    { "UnrecognizedAttribute", 285 },
    { "UnrecognizedChannelType", 286 },
    { "UnrecognizedColor", 287 },
    { "UnrecognizedColormapType", 288 },
    { "UnrecognizedColorspace", 289 },
    { "UnrecognizedCommand", 290 },
    { "UnrecognizedComposeOperator", 291 },
    { "UnrecognizedDisposeMethod", 292 },
    { "UnrecognizedElement", 293 },
    { "UnrecognizedEndianType", 294 },
    { "UnrecognizedGravityType", 295 },
    { "UnrecognizedHighlightStyle", 296 },
    { "UnrecognizedImageCompression", 297 },
    { "UnrecognizedImageFilter", 298 },
    { "UnrecognizedImageFormat", 299 },
    { "UnrecognizedImageMode", 300 },
    { "UnrecognizedImageType", 301 },
    { "UnrecognizedIntentType", 302 },
    { "UnrecognizedInterlaceType", 303 },
    { "UnrecognizedListType", 304 },
    { "UnrecognizedMetric", 305 },
    { "UnrecognizedModeType", 306 },
    { "UnrecognizedNoiseType", 307 },
    { "UnrecognizedOperator", 308 },
    { "UnrecognizedOption", 309 },
    { "UnrecognizedPerlMagickMethod", 310 },
    { "UnrecognizedPixelMap", 311 },
    { "UnrecognizedPreviewType", 312 },
    { "UnrecognizedResourceType", 313 },
    { "UnrecognizedType", 314 },
    { "UnrecognizedUnitsType", 315 },
    { "UnrecognizedVirtualPixelMethod", 316 },
    { "UnsupportedSamplingFactor", 317 },
    { "UsageError", 318 },
    { "InvalidColorspaceType", 319 },
    { "InvalidEndianType", 320 },
    { "InvalidImageType", 321 },
    { "InvalidInterlaceType", 322 },
    { "MissingAnImageFilename", 323 },
    { "MissingArgument", 324 },
    { "NoImagesWereLoaded", 325 },
    { "OptionLengthExceedsLimit", 326 },
    { "RequestDidNotReturnAnImage", 327 },
    { "UnableToOpenXServer", 328 },
    { "UnableToPersistKey", 329 },
    { "UnrecognizedColormapType", 330 },
    { "UnrecognizedColorspaceType", 331 },
    { "UnrecognizedDisposeMethod", 332 },
    { "UnrecognizedEndianType", 333 },
    { "UnrecognizedFilterType", 334 },
    { "UnrecognizedImageCompressionType", 335 },
    { "UnrecognizedImageType", 336 },
    { "UnrecognizedInterlaceType", 337 },
    { "UnrecognizedOption", 338 },
    { "UnrecognizedResourceType", 339 },
    { "UnrecognizedVirtualPixelMethod", 340 },
    { "UnrecognizedColor", 341 },
    { "ImageExpected", 342 },
    { "ImageInfoExpected", 343 },
    { "StructureSizeMismatch", 344 },
    { "UnableToGetRegistryID", 345 },
    { "UnableToLocateImage", 346 },
    { "UnableToSetRegistry", 347 },
    { "Default", 348 },
    { "Default", 349 },
    { "CacheResourcesExhausted", 350 },
    { "ImagePixelLimitExceeded", 351 },
    { "MemoryAllocationFailed", 352 },
    { "NoPixelsDefinedInCache", 353 },
    { "PixelCacheAllocationFailed", 354 },
    { "UnableToAddColorProfile", 355 },
    { "UnableToAddGenericProfile", 356 },
    { "UnableToAddIPTCProfile", 357 },
    { "UnableToAllocateCoefficients", 358 },
    { "UnableToAllocateColormap", 359 },
    { "UnableToAllocateICCProfile", 360 },
    { "UnableToAllocateString", 361 },
    { "UnableToAnnotateImage", 362 },
    { "UnableToAverageImageSequence", 363 },
    { "UnableToCloneDrawingWand", 364 },
    { "UnableToCloneImage", 365 },
    { "UnableToComputeImageSignature", 366 },
    { "UnableToConstituteImage", 367 },
    { "UnableToConvertFont", 368 },
    { "UnableToConvertStringToTokens", 369 },
    { "UnableToCreateColormap", 370 },
    { "UnableToCreateColorTransform", 371 },
    { "UnableToCreateCommandWidget", 372 },
    { "UnableToCreateImageGroup", 373 },
    { "UnableToCreateImageMontage", 374 },
    { "UnableToCreateXWindow", 375 },
    { "UnableToCropImage", 376 },
    { "UnableToDespeckleImage", 377 },
    { "UnableToDetermineImageClass", 378 },
    { "UnableToDetermineTheNumberOfImageColors", 379 },
    { "UnableToDitherImage", 380 },
    { "UnableToDrawOnImage", 381 },
    { "UnableToEdgeImage", 382 },
    { "UnableToEmbossImage", 383 },
    { "UnableToEnhanceImage", 384 },
    { "UnableToFloodfillImage", 385 },
    { "UnableToGammaCorrectImage", 386 },
    { "UnableToGetBestIconSize", 387 },
    { "UnableToGetFromRegistry", 388 },
    { "UnableToGetPackageInfo", 389 },
    { "UnableToLevelImage", 390 },
    { "UnableToMagnifyImage", 391 },
    { "UnableToManageColor", 392 },
    { "UnableToMapImage", 393 },
    { "UnableToMapImageSequence", 394 },
    { "UnableToMedianFilterImage", 395 },
    { "UnableToMotionBlurImage", 396 },
    { "UnableToNoiseFilterImage", 397 },
    { "UnableToNormalizeImage", 398 },
    { "UnableToOpenColorProfile", 399 },
    { "UnableToQuantizeImage", 400 },
    { "UnableToQuantizeImageSequence", 401 },
    { "UnableToReadTextChunk", 402 },
    { "UnableToReadXImage", 403 },
    { "UnableToReadXServerColormap", 404 },
    { "UnableToResizeImage", 405 },
    { "UnableToRotateImage", 406 },
    { "UnableToSampleImage", 407 },
    { "UnableToScaleImage", 408 },
    { "UnableToSelectImage", 409 },
    { "UnableToSharpenImage", 410 },
    { "UnableToShaveImage", 411 },
    { "UnableToShearImage", 412 },
    { "UnableToSortImageColormap", 413 },
    { "UnableToThresholdImage", 414 },
    { "UnableToTransformColorspace", 415 },
    { "MemoryAllocationFailed", 416 },
    { "SemaporeOperationFailed", 417 },
    { "UnableToAllocateAscii85Info", 418 },
    { "UnableToAllocateCacheInfo", 419 },
    { "UnableToAllocateCacheView", 420 },
    { "UnableToAllocateColorInfo", 421 },
    { "UnableToAllocateDashPattern", 422 },
    { "UnableToAllocateDelegateInfo", 423 },
    { "UnableToAllocateDerivatives", 424 },
    { "UnableToAllocateDrawContext", 425 },
    { "UnableToAllocateDrawInfo", 426 },
    { "UnableToAllocateDrawingWand", 427 },
    { "UnableToAllocateGammaMap", 428 },
    { "UnableToAllocateImage", 429 },
    { "UnableToAllocateImagePixels", 430 },
    { "UnableToAllocateLogInfo", 431 },
    { "UnableToAllocateMagicInfo", 432 },
    { "UnableToAllocateMagickInfo", 433 },
    { "UnableToAllocateModuleInfo", 434 },
    { "UnableToAllocateMontageInfo", 435 },
    { "UnableToAllocateQuantizeInfo", 436 },
    { "UnableToAllocateRandomKernel", 437 },
    { "UnableToAllocateRegistryInfo", 438 },
    { "UnableToAllocateSemaphoreInfo", 439 },
    { "UnableToAllocateString", 440 },
    { "UnableToAllocateTypeInfo", 441 },
    { "UnableToAllocateWand", 442 },
    { "UnableToAnimateImageSequence", 443 },
    { "UnableToCloneBlobInfo", 444 },
    { "UnableToCloneCacheInfo", 445 },
    { "UnableToCloneImage", 446 },
    { "UnableToCloneImageInfo", 447 },
    { "UnableToConcatenateString", 448 },
    { "UnableToConvertText", 449 },
    { "UnableToCreateColormap", 450 },
    { "UnableToDestroySemaphore", 451 },
    { "UnableToDisplayImage", 452 },
    { "UnableToEscapeString", 453 },
    { "UnableToInitializeSemaphore", 454 },
    { "UnableToInterpretMSLImage", 455 },
    { "UnableToLockSemaphore", 456 },
    { "UnableToUnlockSemaphore", 457 },
    { "MemoryAllocationFailed", 458 },
    { "ImageDoesNotContainTheStreamGeometry", 459 },
    { "NoStreamHandlerIsDefined", 460 },
    { "PixelCacheIsNotOpen", 461 },
    { "UnableToAcquirePixelStream", 462 },
    { "UnableToSetPixelStream", 463 },
    { "UnableToSyncPixelStream", 464 },
    { "Default", 465 },
    { "Default", 466 },
    { "FontSubstitutionRequired", 467 },
    { "UnableToGetTypeMetrics", 468 },
    { "UnableToInitializeFreetypeLibrary", 469 },
    { "UnableToReadFont", 470 },
    { "UnrecognizedFontEncoding", 471 },
    { "Default", 472 },
    { "Default", 473 },
    { "InvalidColormapIndex", 474 },
    { "WandAPINotImplemented", 475 },
    { "WandContainsNoImageIndexs", 476 },
    { "WandContainsNoImages", 477 },
    { "ColorIsNotKnownToServer", 478 },
    { "NoWindowWithSpecifiedIDExists", 479 },
    { "StandardColormapIsNotInitialized", 480 },
    { "UnableToConnectToRemoteDisplay", 481 },
    { "UnableToCreateBitmap", 482 },
    { "UnableToCreateColormap", 483 },
    { "UnableToCreatePixmap", 484 },
    { "UnableToCreateProperty", 485 },
    { "UnableToCreateStandardColormap", 486 },
    { "UnableToDisplayImageInfo", 487 },
    { "UnableToGetProperty", 488 },
    { "UnableToGetStandardColormap", 489 },
    { "UnableToGetVisual", 490 },
    { "UnableToGrabMouse", 491 },
    { "UnableToLoadFont", 492 },
    { "UnableToMatchVisualToStandardColormap", 493 },
    { "UnableToOpenXServer", 494 },
    { "UnableToReadXAttributes", 495 },
    { "UnableToReadXWindowImage", 496 },
    { "UnrecognizedColormapType", 497 },
    { "UnrecognizedGravityType", 498 },
    { "UnrecognizedVisualSpecifier", 499 },
    { "UnableToAllocateXHints", 500 },
    { "UnableToCreateCursor", 501 },
    { "UnableToCreateGraphicContext", 502 },
    { "UnableToCreateStandardColormap", 503 },
    { "UnableToCreateTextProperty", 504 },
    { "UnableToCreateXImage", 505 },
    { "UnableToCreateXPixmap", 506 },
    { "UnableToCreateXWindow", 507 },
    { "UnableToDisplayImage", 508 },
    { "UnableToDitherImage", 509 },
    { "UnableToGetPixelInfo", 510 },
    { "UnableToGetVisual", 511 },
    { "UnableToLoadFont", 512 },
    { "UnableToMakeXWindow", 513 },
    { "UnableToOpenXServer", 514 },
    { "UnableToViewFonts", 515 },
    { "UnableToGetVisual", 516 },
    { "UsingDefaultVisual", 517 },
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
    "Unable to sync cache (check temporary file disk space)",
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
    "String token maximum length exceeded",
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
    "Length and filesize do not match",
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
    "image colorspace mismatch",
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
    "unable to handle image channel",
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
    "default error",
    "default error",
    "User requested termination (via signal)",
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
    "Invalid subimage specification",
    "kernel radius is too small",
    "kernel width must be an odd number",
    "Matrix is not square (%s elements)",
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
    "Semaphore operation failed",
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
    "unable to allocate wand",
    "unable to animate image sequence",
    "unable to clone blob info",
    "unable to clone cache info",
    "unable to clone image",
    "unable to clone image info",
    "unable to concatenate string",
    "unable to convert text",
    "unable to create colormap",
    "unable to destroy semaphore",
    "unable to display image",
    "unable to escape string",
    "unable to initialize semaphore",
    "unable to interpret MSL image",
    "unable to lock semaphore",
    "unable to unlock semaphore",
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
