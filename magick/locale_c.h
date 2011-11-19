#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 518

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
#define MGK_OptionErrorRegionAreaExceedsLimit 263
#define MGK_OptionErrorRequestDidNotReturnAnImage 264
#define MGK_OptionErrorSteganoImageRequired 265
#define MGK_OptionErrorStereoImageRequired 266
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 267
#define MGK_OptionErrorUnableToAddOrRemoveProfile 268
#define MGK_OptionErrorUnableToAverageImageSequence 269
#define MGK_OptionErrorUnableToBlurImage 270
#define MGK_OptionErrorUnableToChopImage 271
#define MGK_OptionErrorUnableToColorMatrixImage 272
#define MGK_OptionErrorUnableToConstituteImage 273
#define MGK_OptionErrorUnableToConvolveImage 274
#define MGK_OptionErrorUnableToEdgeImage 275
#define MGK_OptionErrorUnableToEqualizeImage 276
#define MGK_OptionErrorUnableToFilterImage 277
#define MGK_OptionErrorUnableToFormatImageMetadata 278
#define MGK_OptionErrorUnableToFrameImage 279
#define MGK_OptionErrorUnableToOilPaintImage 280
#define MGK_OptionErrorUnableToPaintImage 281
#define MGK_OptionErrorUnableToRaiseImage 282
#define MGK_OptionErrorUnableToSharpenImage 283
#define MGK_OptionErrorUnableToThresholdImage 284
#define MGK_OptionErrorUnableToWaveImage 285
#define MGK_OptionErrorUnrecognizedAttribute 286
#define MGK_OptionErrorUnrecognizedChannelType 287
#define MGK_OptionErrorUnrecognizedColor 288
#define MGK_OptionErrorUnrecognizedColormapType 289
#define MGK_OptionErrorUnrecognizedColorspace 290
#define MGK_OptionErrorUnrecognizedCommand 291
#define MGK_OptionErrorUnrecognizedComposeOperator 292
#define MGK_OptionErrorUnrecognizedDisposeMethod 293
#define MGK_OptionErrorUnrecognizedElement 294
#define MGK_OptionErrorUnrecognizedEndianType 295
#define MGK_OptionErrorUnrecognizedGravityType 296
#define MGK_OptionErrorUnrecognizedHighlightStyle 297
#define MGK_OptionErrorUnrecognizedImageCompression 298
#define MGK_OptionErrorUnrecognizedImageFilter 299
#define MGK_OptionErrorUnrecognizedImageFormat 300
#define MGK_OptionErrorUnrecognizedImageMode 301
#define MGK_OptionErrorUnrecognizedImageType 302
#define MGK_OptionErrorUnrecognizedIntentType 303
#define MGK_OptionErrorUnrecognizedInterlaceType 304
#define MGK_OptionErrorUnrecognizedListType 305
#define MGK_OptionErrorUnrecognizedMetric 306
#define MGK_OptionErrorUnrecognizedModeType 307
#define MGK_OptionErrorUnrecognizedNoiseType 308
#define MGK_OptionErrorUnrecognizedOperator 309
#define MGK_OptionErrorUnrecognizedOption 310
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 311
#define MGK_OptionErrorUnrecognizedPixelMap 312
#define MGK_OptionErrorUnrecognizedPreviewType 313
#define MGK_OptionErrorUnrecognizedResourceType 314
#define MGK_OptionErrorUnrecognizedType 315
#define MGK_OptionErrorUnrecognizedUnitsType 316
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 317
#define MGK_OptionErrorUnsupportedSamplingFactor 318
#define MGK_OptionErrorUsageError 319
#define MGK_OptionFatalErrorInvalidColorspaceType 320
#define MGK_OptionFatalErrorInvalidEndianType 321
#define MGK_OptionFatalErrorInvalidImageType 322
#define MGK_OptionFatalErrorInvalidInterlaceType 323
#define MGK_OptionFatalErrorMissingAnImageFilename 324
#define MGK_OptionFatalErrorMissingArgument 325
#define MGK_OptionFatalErrorNoImagesWereLoaded 326
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 327
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 328
#define MGK_OptionFatalErrorUnableToOpenXServer 329
#define MGK_OptionFatalErrorUnableToPersistKey 330
#define MGK_OptionFatalErrorUnrecognizedColormapType 331
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 332
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 333
#define MGK_OptionFatalErrorUnrecognizedEndianType 334
#define MGK_OptionFatalErrorUnrecognizedFilterType 335
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 336
#define MGK_OptionFatalErrorUnrecognizedImageType 337
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 338
#define MGK_OptionFatalErrorUnrecognizedOption 339
#define MGK_OptionFatalErrorUnrecognizedResourceType 340
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 341
#define MGK_OptionWarningUnrecognizedColor 342
#define MGK_RegistryErrorImageExpected 343
#define MGK_RegistryErrorImageInfoExpected 344
#define MGK_RegistryErrorStructureSizeMismatch 345
#define MGK_RegistryErrorUnableToGetRegistryID 346
#define MGK_RegistryErrorUnableToLocateImage 347
#define MGK_RegistryErrorUnableToSetRegistry 348
#define MGK_RegistryFatalErrorDefault 349
#define MGK_RegistryWarningDefault 350
#define MGK_ResourceLimitErrorCacheResourcesExhausted 351
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 352
#define MGK_ResourceLimitErrorMemoryAllocationFailed 353
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 354
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 355
#define MGK_ResourceLimitErrorUnableToAddColorProfile 356
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 357
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 358
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 359
#define MGK_ResourceLimitErrorUnableToAllocateColormap 360
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 361
#define MGK_ResourceLimitErrorUnableToAllocateString 362
#define MGK_ResourceLimitErrorUnableToAnnotateImage 363
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 364
#define MGK_ResourceLimitErrorUnableToCloneDrawingWand 365
#define MGK_ResourceLimitErrorUnableToCloneImage 366
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 367
#define MGK_ResourceLimitErrorUnableToConstituteImage 368
#define MGK_ResourceLimitErrorUnableToConvertFont 369
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 370
#define MGK_ResourceLimitErrorUnableToCreateColormap 371
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 372
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 373
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 374
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 375
#define MGK_ResourceLimitErrorUnableToCreateXWindow 376
#define MGK_ResourceLimitErrorUnableToCropImage 377
#define MGK_ResourceLimitErrorUnableToDespeckleImage 378
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 379
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 380
#define MGK_ResourceLimitErrorUnableToDitherImage 381
#define MGK_ResourceLimitErrorUnableToDrawOnImage 382
#define MGK_ResourceLimitErrorUnableToEdgeImage 383
#define MGK_ResourceLimitErrorUnableToEmbossImage 384
#define MGK_ResourceLimitErrorUnableToEnhanceImage 385
#define MGK_ResourceLimitErrorUnableToFloodfillImage 386
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 387
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 388
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 389
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 390
#define MGK_ResourceLimitErrorUnableToLevelImage 391
#define MGK_ResourceLimitErrorUnableToMagnifyImage 392
#define MGK_ResourceLimitErrorUnableToManageColor 393
#define MGK_ResourceLimitErrorUnableToMapImage 394
#define MGK_ResourceLimitErrorUnableToMapImageSequence 395
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 396
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 397
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 398
#define MGK_ResourceLimitErrorUnableToNormalizeImage 399
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 400
#define MGK_ResourceLimitErrorUnableToQuantizeImage 401
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 402
#define MGK_ResourceLimitErrorUnableToReadTextChunk 403
#define MGK_ResourceLimitErrorUnableToReadXImage 404
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 405
#define MGK_ResourceLimitErrorUnableToResizeImage 406
#define MGK_ResourceLimitErrorUnableToRotateImage 407
#define MGK_ResourceLimitErrorUnableToSampleImage 408
#define MGK_ResourceLimitErrorUnableToScaleImage 409
#define MGK_ResourceLimitErrorUnableToSelectImage 410
#define MGK_ResourceLimitErrorUnableToSharpenImage 411
#define MGK_ResourceLimitErrorUnableToShaveImage 412
#define MGK_ResourceLimitErrorUnableToShearImage 413
#define MGK_ResourceLimitErrorUnableToSortImageColormap 414
#define MGK_ResourceLimitErrorUnableToThresholdImage 415
#define MGK_ResourceLimitErrorUnableToTransformColorspace 416
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 417
#define MGK_ResourceLimitFatalErrorSemaporeOperationFailed 418
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 419
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 420
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 421
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 422
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 423
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 424
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 425
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 426
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 427
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 428
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 429
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 430
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 431
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 432
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 433
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 434
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 435
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 436
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 437
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 438
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 439
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 440
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 441
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 442
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 443
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 444
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 445
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 446
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 447
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 448
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 449
#define MGK_ResourceLimitFatalErrorUnableToConvertText 450
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 451
#define MGK_ResourceLimitFatalErrorUnableToDestroySemaphore 452
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 453
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 454
#define MGK_ResourceLimitFatalErrorUnableToInitializeSemaphore 455
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 456
#define MGK_ResourceLimitFatalErrorUnableToLockSemaphore 457
#define MGK_ResourceLimitFatalErrorUnableToUnlockSemaphore 458
#define MGK_ResourceLimitWarningMemoryAllocationFailed 459
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 460
#define MGK_StreamErrorNoStreamHandlerIsDefined 461
#define MGK_StreamErrorPixelCacheIsNotOpen 462
#define MGK_StreamErrorUnableToAcquirePixelStream 463
#define MGK_StreamErrorUnableToSetPixelStream 464
#define MGK_StreamErrorUnableToSyncPixelStream 465
#define MGK_StreamFatalErrorDefault 466
#define MGK_StreamWarningDefault 467
#define MGK_TypeErrorFontSubstitutionRequired 468
#define MGK_TypeErrorUnableToGetTypeMetrics 469
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 470
#define MGK_TypeErrorUnableToReadFont 471
#define MGK_TypeErrorUnrecognizedFontEncoding 472
#define MGK_TypeFatalErrorDefault 473
#define MGK_TypeWarningDefault 474
#define MGK_WandErrorInvalidColormapIndex 475
#define MGK_WandErrorWandAPINotImplemented 476
#define MGK_WandErrorWandContainsNoImageIndexs 477
#define MGK_WandErrorWandContainsNoImages 478
#define MGK_XServerErrorColorIsNotKnownToServer 479
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 480
#define MGK_XServerErrorStandardColormapIsNotInitialized 481
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 482
#define MGK_XServerErrorUnableToCreateBitmap 483
#define MGK_XServerErrorUnableToCreateColormap 484
#define MGK_XServerErrorUnableToCreatePixmap 485
#define MGK_XServerErrorUnableToCreateProperty 486
#define MGK_XServerErrorUnableToCreateStandardColormap 487
#define MGK_XServerErrorUnableToDisplayImageInfo 488
#define MGK_XServerErrorUnableToGetProperty 489
#define MGK_XServerErrorUnableToGetStandardColormap 490
#define MGK_XServerErrorUnableToGetVisual 491
#define MGK_XServerErrorUnableToGrabMouse 492
#define MGK_XServerErrorUnableToLoadFont 493
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 494
#define MGK_XServerErrorUnableToOpenXServer 495
#define MGK_XServerErrorUnableToReadXAttributes 496
#define MGK_XServerErrorUnableToReadXWindowImage 497
#define MGK_XServerErrorUnrecognizedColormapType 498
#define MGK_XServerErrorUnrecognizedGravityType 499
#define MGK_XServerErrorUnrecognizedVisualSpecifier 500
#define MGK_XServerFatalErrorUnableToAllocateXHints 501
#define MGK_XServerFatalErrorUnableToCreateCursor 502
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 503
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 504
#define MGK_XServerFatalErrorUnableToCreateTextProperty 505
#define MGK_XServerFatalErrorUnableToCreateXImage 506
#define MGK_XServerFatalErrorUnableToCreateXPixmap 507
#define MGK_XServerFatalErrorUnableToCreateXWindow 508
#define MGK_XServerFatalErrorUnableToDisplayImage 509
#define MGK_XServerFatalErrorUnableToDitherImage 510
#define MGK_XServerFatalErrorUnableToGetPixelInfo 511
#define MGK_XServerFatalErrorUnableToGetVisual 512
#define MGK_XServerFatalErrorUnableToLoadFont 513
#define MGK_XServerFatalErrorUnableToMakeXWindow 514
#define MGK_XServerFatalErrorUnableToOpenXServer 515
#define MGK_XServerFatalErrorUnableToViewFonts 516
#define MGK_XServerWarningUnableToGetVisual 517
#define MGK_XServerWarningUsingDefaultVisual 518

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
    { "Option/FatalError", 319, OptionFatalError },
    { "Option/Warning", 341, OptionWarning },
    { "Registry/Error", 342, RegistryError },
    { "Registry/FatalError", 348, RegistryFatalError },
    { "Registry/Warning", 349, RegistryWarning },
    { "Resource/Limit/Error", 350, ResourceLimitError },
    { "Resource/Limit/FatalError", 416, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 458, ResourceLimitWarning },
    { "Stream/Error", 459, StreamError },
    { "Stream/FatalError", 465, StreamFatalError },
    { "Stream/Warning", 466, StreamWarning },
    { "Type/Error", 467, TypeError },
    { "Type/FatalError", 472, TypeFatalError },
    { "Type/Warning", 473, TypeWarning },
    { "Wand/Error", 474, WandError },
    { "XServer/Error", 478, XServerError },
    { "XServer/FatalError", 500, XServerFatalError },
    { "XServer/Warning", 516, XServerWarning },
    { 0, 518, UndefinedException }
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
    { "RegionAreaExceedsLimit", 263 },
    { "RequestDidNotReturnAnImage", 264 },
    { "SteganoImageRequired", 265 },
    { "StereoImageRequired", 266 },
    { "SubimageSpecificationReturnsNoImages", 267 },
    { "UnableToAddOrRemoveProfile", 268 },
    { "UnableToAverageImageSequence", 269 },
    { "UnableToBlurImage", 270 },
    { "UnableToChopImage", 271 },
    { "UnableToColorMatrixImage", 272 },
    { "UnableToConstituteImage", 273 },
    { "UnableToConvolveImage", 274 },
    { "UnableToEdgeImage", 275 },
    { "UnableToEqualizeImage", 276 },
    { "UnableToFilterImage", 277 },
    { "UnableToFormatImageMetadata", 278 },
    { "UnableToFrameImage", 279 },
    { "UnableToOilPaintImage", 280 },
    { "UnableToPaintImage", 281 },
    { "UnableToRaiseImage", 282 },
    { "UnableToSharpenImage", 283 },
    { "UnableToThresholdImage", 284 },
    { "UnableToWaveImage", 285 },
    { "UnrecognizedAttribute", 286 },
    { "UnrecognizedChannelType", 287 },
    { "UnrecognizedColor", 288 },
    { "UnrecognizedColormapType", 289 },
    { "UnrecognizedColorspace", 290 },
    { "UnrecognizedCommand", 291 },
    { "UnrecognizedComposeOperator", 292 },
    { "UnrecognizedDisposeMethod", 293 },
    { "UnrecognizedElement", 294 },
    { "UnrecognizedEndianType", 295 },
    { "UnrecognizedGravityType", 296 },
    { "UnrecognizedHighlightStyle", 297 },
    { "UnrecognizedImageCompression", 298 },
    { "UnrecognizedImageFilter", 299 },
    { "UnrecognizedImageFormat", 300 },
    { "UnrecognizedImageMode", 301 },
    { "UnrecognizedImageType", 302 },
    { "UnrecognizedIntentType", 303 },
    { "UnrecognizedInterlaceType", 304 },
    { "UnrecognizedListType", 305 },
    { "UnrecognizedMetric", 306 },
    { "UnrecognizedModeType", 307 },
    { "UnrecognizedNoiseType", 308 },
    { "UnrecognizedOperator", 309 },
    { "UnrecognizedOption", 310 },
    { "UnrecognizedPerlMagickMethod", 311 },
    { "UnrecognizedPixelMap", 312 },
    { "UnrecognizedPreviewType", 313 },
    { "UnrecognizedResourceType", 314 },
    { "UnrecognizedType", 315 },
    { "UnrecognizedUnitsType", 316 },
    { "UnrecognizedVirtualPixelMethod", 317 },
    { "UnsupportedSamplingFactor", 318 },
    { "UsageError", 319 },
    { "InvalidColorspaceType", 320 },
    { "InvalidEndianType", 321 },
    { "InvalidImageType", 322 },
    { "InvalidInterlaceType", 323 },
    { "MissingAnImageFilename", 324 },
    { "MissingArgument", 325 },
    { "NoImagesWereLoaded", 326 },
    { "OptionLengthExceedsLimit", 327 },
    { "RequestDidNotReturnAnImage", 328 },
    { "UnableToOpenXServer", 329 },
    { "UnableToPersistKey", 330 },
    { "UnrecognizedColormapType", 331 },
    { "UnrecognizedColorspaceType", 332 },
    { "UnrecognizedDisposeMethod", 333 },
    { "UnrecognizedEndianType", 334 },
    { "UnrecognizedFilterType", 335 },
    { "UnrecognizedImageCompressionType", 336 },
    { "UnrecognizedImageType", 337 },
    { "UnrecognizedInterlaceType", 338 },
    { "UnrecognizedOption", 339 },
    { "UnrecognizedResourceType", 340 },
    { "UnrecognizedVirtualPixelMethod", 341 },
    { "UnrecognizedColor", 342 },
    { "ImageExpected", 343 },
    { "ImageInfoExpected", 344 },
    { "StructureSizeMismatch", 345 },
    { "UnableToGetRegistryID", 346 },
    { "UnableToLocateImage", 347 },
    { "UnableToSetRegistry", 348 },
    { "Default", 349 },
    { "Default", 350 },
    { "CacheResourcesExhausted", 351 },
    { "ImagePixelLimitExceeded", 352 },
    { "MemoryAllocationFailed", 353 },
    { "NoPixelsDefinedInCache", 354 },
    { "PixelCacheAllocationFailed", 355 },
    { "UnableToAddColorProfile", 356 },
    { "UnableToAddGenericProfile", 357 },
    { "UnableToAddIPTCProfile", 358 },
    { "UnableToAllocateCoefficients", 359 },
    { "UnableToAllocateColormap", 360 },
    { "UnableToAllocateICCProfile", 361 },
    { "UnableToAllocateString", 362 },
    { "UnableToAnnotateImage", 363 },
    { "UnableToAverageImageSequence", 364 },
    { "UnableToCloneDrawingWand", 365 },
    { "UnableToCloneImage", 366 },
    { "UnableToComputeImageSignature", 367 },
    { "UnableToConstituteImage", 368 },
    { "UnableToConvertFont", 369 },
    { "UnableToConvertStringToTokens", 370 },
    { "UnableToCreateColormap", 371 },
    { "UnableToCreateColorTransform", 372 },
    { "UnableToCreateCommandWidget", 373 },
    { "UnableToCreateImageGroup", 374 },
    { "UnableToCreateImageMontage", 375 },
    { "UnableToCreateXWindow", 376 },
    { "UnableToCropImage", 377 },
    { "UnableToDespeckleImage", 378 },
    { "UnableToDetermineImageClass", 379 },
    { "UnableToDetermineTheNumberOfImageColors", 380 },
    { "UnableToDitherImage", 381 },
    { "UnableToDrawOnImage", 382 },
    { "UnableToEdgeImage", 383 },
    { "UnableToEmbossImage", 384 },
    { "UnableToEnhanceImage", 385 },
    { "UnableToFloodfillImage", 386 },
    { "UnableToGammaCorrectImage", 387 },
    { "UnableToGetBestIconSize", 388 },
    { "UnableToGetFromRegistry", 389 },
    { "UnableToGetPackageInfo", 390 },
    { "UnableToLevelImage", 391 },
    { "UnableToMagnifyImage", 392 },
    { "UnableToManageColor", 393 },
    { "UnableToMapImage", 394 },
    { "UnableToMapImageSequence", 395 },
    { "UnableToMedianFilterImage", 396 },
    { "UnableToMotionBlurImage", 397 },
    { "UnableToNoiseFilterImage", 398 },
    { "UnableToNormalizeImage", 399 },
    { "UnableToOpenColorProfile", 400 },
    { "UnableToQuantizeImage", 401 },
    { "UnableToQuantizeImageSequence", 402 },
    { "UnableToReadTextChunk", 403 },
    { "UnableToReadXImage", 404 },
    { "UnableToReadXServerColormap", 405 },
    { "UnableToResizeImage", 406 },
    { "UnableToRotateImage", 407 },
    { "UnableToSampleImage", 408 },
    { "UnableToScaleImage", 409 },
    { "UnableToSelectImage", 410 },
    { "UnableToSharpenImage", 411 },
    { "UnableToShaveImage", 412 },
    { "UnableToShearImage", 413 },
    { "UnableToSortImageColormap", 414 },
    { "UnableToThresholdImage", 415 },
    { "UnableToTransformColorspace", 416 },
    { "MemoryAllocationFailed", 417 },
    { "SemaporeOperationFailed", 418 },
    { "UnableToAllocateAscii85Info", 419 },
    { "UnableToAllocateCacheInfo", 420 },
    { "UnableToAllocateCacheView", 421 },
    { "UnableToAllocateColorInfo", 422 },
    { "UnableToAllocateDashPattern", 423 },
    { "UnableToAllocateDelegateInfo", 424 },
    { "UnableToAllocateDerivatives", 425 },
    { "UnableToAllocateDrawContext", 426 },
    { "UnableToAllocateDrawInfo", 427 },
    { "UnableToAllocateDrawingWand", 428 },
    { "UnableToAllocateGammaMap", 429 },
    { "UnableToAllocateImage", 430 },
    { "UnableToAllocateImagePixels", 431 },
    { "UnableToAllocateLogInfo", 432 },
    { "UnableToAllocateMagicInfo", 433 },
    { "UnableToAllocateMagickInfo", 434 },
    { "UnableToAllocateModuleInfo", 435 },
    { "UnableToAllocateMontageInfo", 436 },
    { "UnableToAllocateQuantizeInfo", 437 },
    { "UnableToAllocateRandomKernel", 438 },
    { "UnableToAllocateRegistryInfo", 439 },
    { "UnableToAllocateSemaphoreInfo", 440 },
    { "UnableToAllocateString", 441 },
    { "UnableToAllocateTypeInfo", 442 },
    { "UnableToAllocateWand", 443 },
    { "UnableToAnimateImageSequence", 444 },
    { "UnableToCloneBlobInfo", 445 },
    { "UnableToCloneCacheInfo", 446 },
    { "UnableToCloneImage", 447 },
    { "UnableToCloneImageInfo", 448 },
    { "UnableToConcatenateString", 449 },
    { "UnableToConvertText", 450 },
    { "UnableToCreateColormap", 451 },
    { "UnableToDestroySemaphore", 452 },
    { "UnableToDisplayImage", 453 },
    { "UnableToEscapeString", 454 },
    { "UnableToInitializeSemaphore", 455 },
    { "UnableToInterpretMSLImage", 456 },
    { "UnableToLockSemaphore", 457 },
    { "UnableToUnlockSemaphore", 458 },
    { "MemoryAllocationFailed", 459 },
    { "ImageDoesNotContainTheStreamGeometry", 460 },
    { "NoStreamHandlerIsDefined", 461 },
    { "PixelCacheIsNotOpen", 462 },
    { "UnableToAcquirePixelStream", 463 },
    { "UnableToSetPixelStream", 464 },
    { "UnableToSyncPixelStream", 465 },
    { "Default", 466 },
    { "Default", 467 },
    { "FontSubstitutionRequired", 468 },
    { "UnableToGetTypeMetrics", 469 },
    { "UnableToInitializeFreetypeLibrary", 470 },
    { "UnableToReadFont", 471 },
    { "UnrecognizedFontEncoding", 472 },
    { "Default", 473 },
    { "Default", 474 },
    { "InvalidColormapIndex", 475 },
    { "WandAPINotImplemented", 476 },
    { "WandContainsNoImageIndexs", 477 },
    { "WandContainsNoImages", 478 },
    { "ColorIsNotKnownToServer", 479 },
    { "NoWindowWithSpecifiedIDExists", 480 },
    { "StandardColormapIsNotInitialized", 481 },
    { "UnableToConnectToRemoteDisplay", 482 },
    { "UnableToCreateBitmap", 483 },
    { "UnableToCreateColormap", 484 },
    { "UnableToCreatePixmap", 485 },
    { "UnableToCreateProperty", 486 },
    { "UnableToCreateStandardColormap", 487 },
    { "UnableToDisplayImageInfo", 488 },
    { "UnableToGetProperty", 489 },
    { "UnableToGetStandardColormap", 490 },
    { "UnableToGetVisual", 491 },
    { "UnableToGrabMouse", 492 },
    { "UnableToLoadFont", 493 },
    { "UnableToMatchVisualToStandardColormap", 494 },
    { "UnableToOpenXServer", 495 },
    { "UnableToReadXAttributes", 496 },
    { "UnableToReadXWindowImage", 497 },
    { "UnrecognizedColormapType", 498 },
    { "UnrecognizedGravityType", 499 },
    { "UnrecognizedVisualSpecifier", 500 },
    { "UnableToAllocateXHints", 501 },
    { "UnableToCreateCursor", 502 },
    { "UnableToCreateGraphicContext", 503 },
    { "UnableToCreateStandardColormap", 504 },
    { "UnableToCreateTextProperty", 505 },
    { "UnableToCreateXImage", 506 },
    { "UnableToCreateXPixmap", 507 },
    { "UnableToCreateXWindow", 508 },
    { "UnableToDisplayImage", 509 },
    { "UnableToDitherImage", 510 },
    { "UnableToGetPixelInfo", 511 },
    { "UnableToGetVisual", 512 },
    { "UnableToLoadFont", 513 },
    { "UnableToMakeXWindow", 514 },
    { "UnableToOpenXServer", 515 },
    { "UnableToViewFonts", 516 },
    { "UnableToGetVisual", 517 },
    { "UsingDefaultVisual", 518 },
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
    "Region area exceeds implementation limit",
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
