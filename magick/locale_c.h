#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 475

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
#define MGK_ImageErrorImageOpacityDiffers 172
#define MGK_ImageErrorImageSequenceIsRequired 173
#define MGK_ImageErrorImageSizeDiffers 174
#define MGK_ImageErrorInvalidColormapIndex 175
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 176
#define MGK_ImageErrorNoImagesWereFound 177
#define MGK_ImageErrorNoImagesWereLoaded 178
#define MGK_ImageErrorNoLocaleImageAttribute 179
#define MGK_ImageErrorTooManyClusters 180
#define MGK_ImageErrorUnableToAppendImage 181
#define MGK_ImageErrorUnableToAssignProfile 182
#define MGK_ImageErrorUnableToAverageImage 183
#define MGK_ImageErrorUnableToCoalesceImage 184
#define MGK_ImageErrorUnableToCompareImages 185
#define MGK_ImageErrorUnableToCreateImageMosaic 186
#define MGK_ImageErrorUnableToCreateStereoImage 187
#define MGK_ImageErrorUnableToDeconstructImageSequence 188
#define MGK_ImageErrorUnableToFlattenImage 189
#define MGK_ImageErrorUnableToResizeImage 190
#define MGK_ImageErrorUnableToSegmentImage 191
#define MGK_ImageErrorUnableToSetClipMask 192
#define MGK_ImageErrorUnableToShearImage 193
#define MGK_ImageErrorWidthOrHeightExceedsLimit 194
#define MGK_ImageFatalErrorUnableToPersistKey 195
#define MGK_ImageWarningDefault 196
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 197
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 198
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 199
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 200
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 201
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 202
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 203
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 204
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 205
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 206
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 207
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 208
#define MGK_MissingDelegateFatalErrorDefault 209
#define MGK_MissingDelegateWarningDefault 210
#define MGK_ModuleErrorFailedToCloseModule 211
#define MGK_ModuleErrorFailedToFindSymbol 212
#define MGK_ModuleErrorUnableToLoadModule 213
#define MGK_ModuleErrorUnableToRegisterImageFormat 214
#define MGK_ModuleErrorUnrecognizedModule 215
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 216
#define MGK_ModuleWarningDefault 217
#define MGK_OptionErrorBevelWidthIsNegative 218
#define MGK_OptionErrorColorSeparatedImageRequired 219
#define MGK_OptionErrorFrameIsLessThanImageSize 220
#define MGK_OptionErrorGeometryDimensionsAreZero 221
#define MGK_OptionErrorGeometryDoesNotContainImage 222
#define MGK_OptionErrorImagesAreNotTheSameSize 223
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 224
#define MGK_OptionErrorImageSmallerThanKernelWidth 225
#define MGK_OptionErrorImageSmallerThanRadius 226
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 227
#define MGK_OptionErrorInputImagesAlreadySpecified 228
#define MGK_OptionErrorKernelRadiusIsTooSmall 229
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 230
#define MGK_OptionErrorMissingAnImageFilename 231
#define MGK_OptionErrorMissingArgument 232
#define MGK_OptionErrorMustSpecifyAnImageName 233
#define MGK_OptionErrorMustSpecifyImageSize 234
#define MGK_OptionErrorNoBlobDefined 235
#define MGK_OptionErrorNoImagesDefined 236
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 237
#define MGK_OptionErrorNoProfileNameWasGiven 238
#define MGK_OptionErrorNullBlobArgument 239
#define MGK_OptionErrorReferenceImageRequired 240
#define MGK_OptionErrorReferenceIsNotMyType 241
#define MGK_OptionErrorSteganoImageRequired 242
#define MGK_OptionErrorStereoImageRequired 243
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 244
#define MGK_OptionErrorUnableToAddOrRemoveProfile 245
#define MGK_OptionErrorUnableToAllocateICCProfile 246
#define MGK_OptionErrorUnableToAverageImageSequence 247
#define MGK_OptionErrorUnableToBlurImage 248
#define MGK_OptionErrorUnableToChopImage 249
#define MGK_OptionErrorUnableToConstituteImage 250
#define MGK_OptionErrorUnableToConvolveImage 251
#define MGK_OptionErrorUnableToEdgeImage 252
#define MGK_OptionErrorUnableToEqualizeImage 253
#define MGK_OptionErrorUnableToFilterImage 254
#define MGK_OptionErrorUnableToFormatImageMetadata 255
#define MGK_OptionErrorUnableToFrameImage 256
#define MGK_OptionErrorUnableToOilPaintImage 257
#define MGK_OptionErrorUnableToPaintImage 258
#define MGK_OptionErrorUnableToRaiseImage 259
#define MGK_OptionErrorUnableToSharpenImage 260
#define MGK_OptionErrorUnableToThresholdImage 261
#define MGK_OptionErrorUnableToWaveImage 262
#define MGK_OptionErrorUnrecognizedAttribute 263
#define MGK_OptionErrorUnrecognizedChannelType 264
#define MGK_OptionErrorUnrecognizedColor 265
#define MGK_OptionErrorUnrecognizedColormapType 266
#define MGK_OptionErrorUnrecognizedColorspace 267
#define MGK_OptionErrorUnrecognizedCommand 268
#define MGK_OptionErrorUnrecognizedComposeOperator 269
#define MGK_OptionErrorUnrecognizedDisposeMethod 270
#define MGK_OptionErrorUnrecognizedElement 271
#define MGK_OptionErrorUnrecognizedEndianType 272
#define MGK_OptionErrorUnrecognizedGravityType 273
#define MGK_OptionErrorUnrecognizedImageCompression 274
#define MGK_OptionErrorUnrecognizedImageFilter 275
#define MGK_OptionErrorUnrecognizedImageFormat 276
#define MGK_OptionErrorUnrecognizedImageMode 277
#define MGK_OptionErrorUnrecognizedImageType 278
#define MGK_OptionErrorUnrecognizedIntentType 279
#define MGK_OptionErrorUnrecognizedInterlaceType 280
#define MGK_OptionErrorUnrecognizedListType 281
#define MGK_OptionErrorUnrecognizedModeType 282
#define MGK_OptionErrorUnrecognizedNoiseType 283
#define MGK_OptionErrorUnrecognizedOption 284
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 285
#define MGK_OptionErrorUnrecognizedPixelMap 286
#define MGK_OptionErrorUnrecognizedPreviewType 287
#define MGK_OptionErrorUnrecognizedResourceType 288
#define MGK_OptionErrorUnrecognizedType 289
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 290
#define MGK_OptionErrorUsageError 291
#define MGK_OptionFatalErrorInvalidColorspaceType 292
#define MGK_OptionFatalErrorInvalidEndianType 293
#define MGK_OptionFatalErrorInvalidImageType 294
#define MGK_OptionFatalErrorInvalidInterlaceType 295
#define MGK_OptionFatalErrorMissingAnImageFilename 296
#define MGK_OptionFatalErrorMissingArgument 297
#define MGK_OptionFatalErrorNoImagesWereLoaded 298
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 299
#define MGK_OptionFatalErrorUnableToOpenXServer 300
#define MGK_OptionFatalErrorUnableToPersistKey 301
#define MGK_OptionFatalErrorUnrecognizedColormapType 302
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 303
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 304
#define MGK_OptionFatalErrorUnrecognizedEndianType 305
#define MGK_OptionFatalErrorUnrecognizedFilterType 306
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 307
#define MGK_OptionFatalErrorUnrecognizedImageType 308
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 309
#define MGK_OptionFatalErrorUnrecognizedOption 310
#define MGK_OptionFatalErrorUnrecognizedResourceType 311
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 312
#define MGK_OptionWarningUnrecognizedColor 313
#define MGK_RegistryErrorImageExpected 314
#define MGK_RegistryErrorImageInfoExpected 315
#define MGK_RegistryErrorStructureSizeMismatch 316
#define MGK_RegistryErrorUnableToGetRegistryID 317
#define MGK_RegistryErrorUnableToLocateImage 318
#define MGK_RegistryErrorUnableToSetRegistry 319
#define MGK_RegistryFatalErrorDefault 320
#define MGK_RegistryWarningDefault 321
#define MGK_ResourceLimitErrorCacheResourcesExhausted 322
#define MGK_ResourceLimitErrorMemoryAllocationFailed 323
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 324
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 325
#define MGK_ResourceLimitErrorUnableToAddColorProfile 326
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 327
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 328
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 329
#define MGK_ResourceLimitErrorUnableToAllocateColormap 330
#define MGK_ResourceLimitErrorUnableToAllocateString 331
#define MGK_ResourceLimitErrorUnableToAnnotateImage 332
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 333
#define MGK_ResourceLimitErrorUnableToCloneImage 334
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 335
#define MGK_ResourceLimitErrorUnableToConstituteImage 336
#define MGK_ResourceLimitErrorUnableToConvertFont 337
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 338
#define MGK_ResourceLimitErrorUnableToCreateColormap 339
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 340
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 341
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 342
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 343
#define MGK_ResourceLimitErrorUnableToCreateXWindow 344
#define MGK_ResourceLimitErrorUnableToCropImage 345
#define MGK_ResourceLimitErrorUnableToDespeckleImage 346
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 347
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 348
#define MGK_ResourceLimitErrorUnableToDitherImage 349
#define MGK_ResourceLimitErrorUnableToDrawOnImage 350
#define MGK_ResourceLimitErrorUnableToEdgeImage 351
#define MGK_ResourceLimitErrorUnableToEmbossImage 352
#define MGK_ResourceLimitErrorUnableToEnhanceImage 353
#define MGK_ResourceLimitErrorUnableToFloodfillImage 354
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 355
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 356
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 357
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 358
#define MGK_ResourceLimitErrorUnableToLevelImage 359
#define MGK_ResourceLimitErrorUnableToMagnifyImage 360
#define MGK_ResourceLimitErrorUnableToManageColor 361
#define MGK_ResourceLimitErrorUnableToMapImage 362
#define MGK_ResourceLimitErrorUnableToMapImageSequence 363
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 364
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 365
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 366
#define MGK_ResourceLimitErrorUnableToNormalizeImage 367
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 368
#define MGK_ResourceLimitErrorUnableToQuantizeImage 369
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 370
#define MGK_ResourceLimitErrorUnableToReadTextChunk 371
#define MGK_ResourceLimitErrorUnableToReadXImage 372
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 373
#define MGK_ResourceLimitErrorUnableToResizeImage 374
#define MGK_ResourceLimitErrorUnableToRotateImage 375
#define MGK_ResourceLimitErrorUnableToSampleImage 376
#define MGK_ResourceLimitErrorUnableToScaleImage 377
#define MGK_ResourceLimitErrorUnableToSelectImage 378
#define MGK_ResourceLimitErrorUnableToSharpenImage 379
#define MGK_ResourceLimitErrorUnableToShaveImage 380
#define MGK_ResourceLimitErrorUnableToShearImage 381
#define MGK_ResourceLimitErrorUnableToSortImageColormap 382
#define MGK_ResourceLimitErrorUnableToThresholdImage 383
#define MGK_ResourceLimitErrorUnableToTransformColorspace 384
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 385
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 386
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 387
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 388
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 389
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 390
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 391
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 392
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 393
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 394
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 395
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 396
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 397
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 398
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 399
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 400
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 401
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 402
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 403
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 404
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 405
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 406
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 407
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 408
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 409
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 410
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 411
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 412
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 413
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 414
#define MGK_ResourceLimitFatalErrorUnableToConvertText 415
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 416
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 417
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 418
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 419
#define MGK_ResourceLimitWarningMemoryAllocationFailed 420
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 421
#define MGK_StreamErrorNoStreamHandlerIsDefined 422
#define MGK_StreamErrorPixelCacheIsNotOpen 423
#define MGK_StreamErrorUnableToAcquirePixelStream 424
#define MGK_StreamErrorUnableToSetPixelStream 425
#define MGK_StreamErrorUnableToSyncPixelStream 426
#define MGK_StreamFatalErrorDefault 427
#define MGK_StreamWarningDefault 428
#define MGK_TypeErrorFontSubstitutionRequired 429
#define MGK_TypeErrorUnableToGetTypeMetrics 430
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 431
#define MGK_TypeErrorUnableToReadFont 432
#define MGK_TypeErrorUnrecognizedFontEncoding 433
#define MGK_TypeFatalErrorDefault 434
#define MGK_TypeWarningDefault 435
#define MGK_XServerErrorColorIsNotKnownToServer 436
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 437
#define MGK_XServerErrorStandardColormapIsNotInitialized 438
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 439
#define MGK_XServerErrorUnableToCreateBitmap 440
#define MGK_XServerErrorUnableToCreateColormap 441
#define MGK_XServerErrorUnableToCreatePixmap 442
#define MGK_XServerErrorUnableToCreateProperty 443
#define MGK_XServerErrorUnableToCreateStandardColormap 444
#define MGK_XServerErrorUnableToDisplayImageInfo 445
#define MGK_XServerErrorUnableToGetProperty 446
#define MGK_XServerErrorUnableToGetStandardColormap 447
#define MGK_XServerErrorUnableToGetVisual 448
#define MGK_XServerErrorUnableToGrabMouse 449
#define MGK_XServerErrorUnableToLoadFont 450
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 451
#define MGK_XServerErrorUnableToOpenXServer 452
#define MGK_XServerErrorUnableToReadXAttributes 453
#define MGK_XServerErrorUnableToReadXWindowImage 454
#define MGK_XServerErrorUnrecognizedColormapType 455
#define MGK_XServerErrorUnrecognizedGravityType 456
#define MGK_XServerErrorUnrecognizedVisualSpecifier 457
#define MGK_XServerFatalErrorUnableToAllocateXHints 458
#define MGK_XServerFatalErrorUnableToCreateCursor 459
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 460
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 461
#define MGK_XServerFatalErrorUnableToCreateTextProperty 462
#define MGK_XServerFatalErrorUnableToCreateXImage 463
#define MGK_XServerFatalErrorUnableToCreateXPixmap 464
#define MGK_XServerFatalErrorUnableToCreateXWindow 465
#define MGK_XServerFatalErrorUnableToDisplayImage 466
#define MGK_XServerFatalErrorUnableToDitherImage 467
#define MGK_XServerFatalErrorUnableToGetPixelInfo 468
#define MGK_XServerFatalErrorUnableToGetVisual 469
#define MGK_XServerFatalErrorUnableToLoadFont 470
#define MGK_XServerFatalErrorUnableToMakeXWindow 471
#define MGK_XServerFatalErrorUnableToOpenXServer 472
#define MGK_XServerFatalErrorUnableToViewFonts 473
#define MGK_XServerWarningUnableToGetVisual 474
#define MGK_XServerWarningUsingDefaultVisual 475

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
    { "Image/FatalError", 194, ImageFatalError },
    { "Image/Warning", 195, ImageWarning },
    { "Missing/Delegate/Error", 196, MissingDelegateError },
    { "Missing/Delegate/FatalError", 208, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 209, MissingDelegateWarning },
    { "Module/Error", 210, ModuleError },
    { "Module/FatalError", 215, ModuleFatalError },
    { "Module/Warning", 216, ModuleWarning },
    { "Option/Error", 217, OptionError },
    { "Option/FatalError", 291, OptionFatalError },
    { "Option/Warning", 312, OptionWarning },
    { "Registry/Error", 313, RegistryError },
    { "Registry/FatalError", 319, RegistryFatalError },
    { "Registry/Warning", 320, RegistryWarning },
    { "Resource/Limit/Error", 321, ResourceLimitError },
    { "Resource/Limit/FatalError", 384, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 419, ResourceLimitWarning },
    { "Stream/Error", 420, StreamError },
    { "Stream/FatalError", 426, StreamFatalError },
    { "Stream/Warning", 427, StreamWarning },
    { "Type/Error", 428, TypeError },
    { "Type/FatalError", 433, TypeFatalError },
    { "Type/Warning", 434, TypeWarning },
    { "XServer/Error", 435, XServerError },
    { "XServer/FatalError", 457, XServerFatalError },
    { "XServer/Warning", 473, XServerWarning },
    { 0, 475, UndefinedException }
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
    { "ImageOpacityDiffers", 172 },
    { "ImageSequenceIsRequired", 173 },
    { "ImageSizeDiffers", 174 },
    { "InvalidColormapIndex", 175 },
    { "LeftAndRightImageSizesDiffer", 176 },
    { "NoImagesWereFound", 177 },
    { "NoImagesWereLoaded", 178 },
    { "NoLocaleImageAttribute", 179 },
    { "TooManyClusters", 180 },
    { "UnableToAppendImage", 181 },
    { "UnableToAssignProfile", 182 },
    { "UnableToAverageImage", 183 },
    { "UnableToCoalesceImage", 184 },
    { "UnableToCompareImages", 185 },
    { "UnableToCreateImageMosaic", 186 },
    { "UnableToCreateStereoImage", 187 },
    { "UnableToDeconstructImageSequence", 188 },
    { "UnableToFlattenImage", 189 },
    { "UnableToResizeImage", 190 },
    { "UnableToSegmentImage", 191 },
    { "UnableToSetClipMask", 192 },
    { "UnableToShearImage", 193 },
    { "WidthOrHeightExceedsLimit", 194 },
    { "UnableToPersistKey", 195 },
    { "Default", 196 },
    { "DPSLibraryIsNotAvailable", 197 },
    { "FPXLibraryIsNotAvailable", 198 },
    { "FreeTypeLibraryIsNotAvailable", 199 },
    { "JPEGLibraryIsNotAvailable", 200 },
    { "LCMSLibraryIsNotAvailable", 201 },
    { "LZWEncodingNotEnabled", 202 },
    { "NoDecodeDelegateForThisImageFormat", 203 },
    { "NoEncodeDelegateForThisImageFormat", 204 },
    { "TIFFLibraryIsNotAvailable", 205 },
    { "XMLLibraryIsNotAvailable", 206 },
    { "XWindowLibraryIsNotAvailable", 207 },
    { "ZipLibraryIsNotAvailable", 208 },
    { "Default", 209 },
    { "Default", 210 },
    { "FailedToCloseModule", 211 },
    { "FailedToFindSymbol", 212 },
    { "UnableToLoadModule", 213 },
    { "UnableToRegisterImageFormat", 214 },
    { "UnrecognizedModule", 215 },
    { "UnableToInitializeModuleLoader", 216 },
    { "Default", 217 },
    { "BevelWidthIsNegative", 218 },
    { "ColorSeparatedImageRequired", 219 },
    { "FrameIsLessThanImageSize", 220 },
    { "GeometryDimensionsAreZero", 221 },
    { "GeometryDoesNotContainImage", 222 },
    { "ImagesAreNotTheSameSize", 223 },
    { "ImageSizeMustExceedBevelWidth", 224 },
    { "ImageSmallerThanKernelWidth", 225 },
    { "ImageSmallerThanRadius", 226 },
    { "ImageWidthsOrHeightsDiffer", 227 },
    { "InputImagesAlreadySpecified", 228 },
    { "KernelRadiusIsTooSmall", 229 },
    { "KernelWidthMustBeAnOddNumber", 230 },
    { "MissingAnImageFilename", 231 },
    { "MissingArgument", 232 },
    { "MustSpecifyAnImageName", 233 },
    { "MustSpecifyImageSize", 234 },
    { "NoBlobDefined", 235 },
    { "NoImagesDefined", 236 },
    { "NonzeroWidthAndHeightRequired", 237 },
    { "NoProfileNameWasGiven", 238 },
    { "NullBlobArgument", 239 },
    { "ReferenceImageRequired", 240 },
    { "ReferenceIsNotMyType", 241 },
    { "SteganoImageRequired", 242 },
    { "StereoImageRequired", 243 },
    { "SubimageSpecificationReturnsNoImages", 244 },
    { "UnableToAddOrRemoveProfile", 245 },
    { "UnableToAllocateICCProfile", 246 },
    { "UnableToAverageImageSequence", 247 },
    { "UnableToBlurImage", 248 },
    { "UnableToChopImage", 249 },
    { "UnableToConstituteImage", 250 },
    { "UnableToConvolveImage", 251 },
    { "UnableToEdgeImage", 252 },
    { "UnableToEqualizeImage", 253 },
    { "UnableToFilterImage", 254 },
    { "UnableToFormatImageMetadata", 255 },
    { "UnableToFrameImage", 256 },
    { "UnableToOilPaintImage", 257 },
    { "UnableToPaintImage", 258 },
    { "UnableToRaiseImage", 259 },
    { "UnableToSharpenImage", 260 },
    { "UnableToThresholdImage", 261 },
    { "UnableToWaveImage", 262 },
    { "UnrecognizedAttribute", 263 },
    { "UnrecognizedChannelType", 264 },
    { "UnrecognizedColor", 265 },
    { "UnrecognizedColormapType", 266 },
    { "UnrecognizedColorspace", 267 },
    { "UnrecognizedCommand", 268 },
    { "UnrecognizedComposeOperator", 269 },
    { "UnrecognizedDisposeMethod", 270 },
    { "UnrecognizedElement", 271 },
    { "UnrecognizedEndianType", 272 },
    { "UnrecognizedGravityType", 273 },
    { "UnrecognizedImageCompression", 274 },
    { "UnrecognizedImageFilter", 275 },
    { "UnrecognizedImageFormat", 276 },
    { "UnrecognizedImageMode", 277 },
    { "UnrecognizedImageType", 278 },
    { "UnrecognizedIntentType", 279 },
    { "UnrecognizedInterlaceType", 280 },
    { "UnrecognizedListType", 281 },
    { "UnrecognizedModeType", 282 },
    { "UnrecognizedNoiseType", 283 },
    { "UnrecognizedOption", 284 },
    { "UnrecognizedPerlMagickMethod", 285 },
    { "UnrecognizedPixelMap", 286 },
    { "UnrecognizedPreviewType", 287 },
    { "UnrecognizedResourceType", 288 },
    { "UnrecognizedType", 289 },
    { "UnrecognizedVirtualPixelMethod", 290 },
    { "UsageError", 291 },
    { "InvalidColorspaceType", 292 },
    { "InvalidEndianType", 293 },
    { "InvalidImageType", 294 },
    { "InvalidInterlaceType", 295 },
    { "MissingAnImageFilename", 296 },
    { "MissingArgument", 297 },
    { "NoImagesWereLoaded", 298 },
    { "OptionLengthExceedsLimit", 299 },
    { "UnableToOpenXServer", 300 },
    { "UnableToPersistKey", 301 },
    { "UnrecognizedColormapType", 302 },
    { "UnrecognizedColorspaceType", 303 },
    { "UnrecognizedDisposeMethod", 304 },
    { "UnrecognizedEndianType", 305 },
    { "UnrecognizedFilterType", 306 },
    { "UnrecognizedImageCompressionType", 307 },
    { "UnrecognizedImageType", 308 },
    { "UnrecognizedInterlaceType", 309 },
    { "UnrecognizedOption", 310 },
    { "UnrecognizedResourceType", 311 },
    { "UnrecognizedVirtualPixelMethod", 312 },
    { "UnrecognizedColor", 313 },
    { "ImageExpected", 314 },
    { "ImageInfoExpected", 315 },
    { "StructureSizeMismatch", 316 },
    { "UnableToGetRegistryID", 317 },
    { "UnableToLocateImage", 318 },
    { "UnableToSetRegistry", 319 },
    { "Default", 320 },
    { "Default", 321 },
    { "CacheResourcesExhausted", 322 },
    { "MemoryAllocationFailed", 323 },
    { "NoPixelsDefinedInCache", 324 },
    { "PixelCacheAllocationFailed", 325 },
    { "UnableToAddColorProfile", 326 },
    { "UnableToAddGenericProfile", 327 },
    { "UnableToAddIPTCProfile", 328 },
    { "UnableToAllocateCoefficients", 329 },
    { "UnableToAllocateColormap", 330 },
    { "UnableToAllocateString", 331 },
    { "UnableToAnnotateImage", 332 },
    { "UnableToAverageImageSequence", 333 },
    { "UnableToCloneImage", 334 },
    { "UnableToComputeImageSignature", 335 },
    { "UnableToConstituteImage", 336 },
    { "UnableToConvertFont", 337 },
    { "UnableToConvertStringToTokens", 338 },
    { "UnableToCreateColormap", 339 },
    { "UnableToCreateColorTransform", 340 },
    { "UnableToCreateCommandWidget", 341 },
    { "UnableToCreateImageGroup", 342 },
    { "UnableToCreateImageMontage", 343 },
    { "UnableToCreateXWindow", 344 },
    { "UnableToCropImage", 345 },
    { "UnableToDespeckleImage", 346 },
    { "UnableToDetermineImageClass", 347 },
    { "UnableToDetermineTheNumberOfImageColors", 348 },
    { "UnableToDitherImage", 349 },
    { "UnableToDrawOnImage", 350 },
    { "UnableToEdgeImage", 351 },
    { "UnableToEmbossImage", 352 },
    { "UnableToEnhanceImage", 353 },
    { "UnableToFloodfillImage", 354 },
    { "UnableToGammaCorrectImage", 355 },
    { "UnableToGetBestIconSize", 356 },
    { "UnableToGetFromRegistry", 357 },
    { "UnableToGetPackageInfo", 358 },
    { "UnableToLevelImage", 359 },
    { "UnableToMagnifyImage", 360 },
    { "UnableToManageColor", 361 },
    { "UnableToMapImage", 362 },
    { "UnableToMapImageSequence", 363 },
    { "UnableToMedianFilterImage", 364 },
    { "UnableToMotionBlurImage", 365 },
    { "UnableToNoiseFilterImage", 366 },
    { "UnableToNormalizeImage", 367 },
    { "UnableToOpenColorProfile", 368 },
    { "UnableToQuantizeImage", 369 },
    { "UnableToQuantizeImageSequence", 370 },
    { "UnableToReadTextChunk", 371 },
    { "UnableToReadXImage", 372 },
    { "UnableToReadXServerColormap", 373 },
    { "UnableToResizeImage", 374 },
    { "UnableToRotateImage", 375 },
    { "UnableToSampleImage", 376 },
    { "UnableToScaleImage", 377 },
    { "UnableToSelectImage", 378 },
    { "UnableToSharpenImage", 379 },
    { "UnableToShaveImage", 380 },
    { "UnableToShearImage", 381 },
    { "UnableToSortImageColormap", 382 },
    { "UnableToThresholdImage", 383 },
    { "UnableToTransformColorspace", 384 },
    { "MemoryAllocationFailed", 385 },
    { "UnableToAllocateAscii85Info", 386 },
    { "UnableToAllocateCacheInfo", 387 },
    { "UnableToAllocateCacheView", 388 },
    { "UnableToAllocateColorInfo", 389 },
    { "UnableToAllocateDashPattern", 390 },
    { "UnableToAllocateDelegateInfo", 391 },
    { "UnableToAllocateDerivatives", 392 },
    { "UnableToAllocateDrawContext", 393 },
    { "UnableToAllocateDrawInfo", 394 },
    { "UnableToAllocateDrawingWand", 395 },
    { "UnableToAllocateGammaMap", 396 },
    { "UnableToAllocateImage", 397 },
    { "UnableToAllocateImagePixels", 398 },
    { "UnableToAllocateLogInfo", 399 },
    { "UnableToAllocateMagicInfo", 400 },
    { "UnableToAllocateMagickInfo", 401 },
    { "UnableToAllocateModuleInfo", 402 },
    { "UnableToAllocateMontageInfo", 403 },
    { "UnableToAllocateQuantizeInfo", 404 },
    { "UnableToAllocateRegistryInfo", 405 },
    { "UnableToAllocateSemaphoreInfo", 406 },
    { "UnableToAllocateString", 407 },
    { "UnableToAllocateTypeInfo", 408 },
    { "UnableToAnimateImageSequence", 409 },
    { "UnableToCloneBlobInfo", 410 },
    { "UnableToCloneCacheInfo", 411 },
    { "UnableToCloneImage", 412 },
    { "UnableToCloneImageInfo", 413 },
    { "UnableToConcatenateString", 414 },
    { "UnableToConvertText", 415 },
    { "UnableToCreateColormap", 416 },
    { "UnableToDisplayImage", 417 },
    { "UnableToEscapeString", 418 },
    { "UnableToInterpretMSLImage", 419 },
    { "MemoryAllocationFailed", 420 },
    { "ImageDoesNotContainTheStreamGeometry", 421 },
    { "NoStreamHandlerIsDefined", 422 },
    { "PixelCacheIsNotOpen", 423 },
    { "UnableToAcquirePixelStream", 424 },
    { "UnableToSetPixelStream", 425 },
    { "UnableToSyncPixelStream", 426 },
    { "Default", 427 },
    { "Default", 428 },
    { "FontSubstitutionRequired", 429 },
    { "UnableToGetTypeMetrics", 430 },
    { "UnableToInitializeFreetypeLibrary", 431 },
    { "UnableToReadFont", 432 },
    { "UnrecognizedFontEncoding", 433 },
    { "Default", 434 },
    { "Default", 435 },
    { "ColorIsNotKnownToServer", 436 },
    { "NoWindowWithSpecifiedIDExists", 437 },
    { "StandardColormapIsNotInitialized", 438 },
    { "UnableToConnectToRemoteDisplay", 439 },
    { "UnableToCreateBitmap", 440 },
    { "UnableToCreateColormap", 441 },
    { "UnableToCreatePixmap", 442 },
    { "UnableToCreateProperty", 443 },
    { "UnableToCreateStandardColormap", 444 },
    { "UnableToDisplayImageInfo", 445 },
    { "UnableToGetProperty", 446 },
    { "UnableToGetStandardColormap", 447 },
    { "UnableToGetVisual", 448 },
    { "UnableToGrabMouse", 449 },
    { "UnableToLoadFont", 450 },
    { "UnableToMatchVisualToStandardColormap", 451 },
    { "UnableToOpenXServer", 452 },
    { "UnableToReadXAttributes", 453 },
    { "UnableToReadXWindowImage", 454 },
    { "UnrecognizedColormapType", 455 },
    { "UnrecognizedGravityType", 456 },
    { "UnrecognizedVisualSpecifier", 457 },
    { "UnableToAllocateXHints", 458 },
    { "UnableToCreateCursor", 459 },
    { "UnableToCreateGraphicContext", 460 },
    { "UnableToCreateStandardColormap", 461 },
    { "UnableToCreateTextProperty", 462 },
    { "UnableToCreateXImage", 463 },
    { "UnableToCreateXPixmap", 464 },
    { "UnableToCreateXWindow", 465 },
    { "UnableToDisplayImage", 466 },
    { "UnableToDitherImage", 467 },
    { "UnableToGetPixelInfo", 468 },
    { "UnableToGetVisual", 469 },
    { "UnableToLoadFont", 470 },
    { "UnableToMakeXWindow", 471 },
    { "UnableToOpenXServer", 472 },
    { "UnableToViewFonts", 473 },
    { "UnableToGetVisual", 474 },
    { "UsingDefaultVisual", 475 },
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
    "Unrecognized option",
    "Unrecognized PerlMagick method",
    "Unrecognized pixel map",
    "Unrecognized preview type",
    "Unrecognized resource type",
    "Unrecognized type",
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
