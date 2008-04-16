#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 491

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
#define MGK_CoderErrorSubsamplingRequiresEvenWidth 61
#define MGK_CoderErrorUnableToCopyProfile 62
#define MGK_CoderErrorUnableToCreateADC 63
#define MGK_CoderErrorUnableToCreateBitmap 64
#define MGK_CoderErrorUnableToDecompressImage 65
#define MGK_CoderErrorUnableToInitializeFPXLibrary 66
#define MGK_CoderErrorUnableToOpenBlob 67
#define MGK_CoderErrorUnableToReadAspectRatio 68
#define MGK_CoderErrorUnableToReadCIELABImages 69
#define MGK_CoderErrorUnableToReadSummaryInfo 70
#define MGK_CoderErrorUnableToSetAffineMatrix 71
#define MGK_CoderErrorUnableToSetAspectRatio 72
#define MGK_CoderErrorUnableToSetColorTwist 73
#define MGK_CoderErrorUnableToSetContrast 74
#define MGK_CoderErrorUnableToSetFilteringValue 75
#define MGK_CoderErrorUnableToSetImageComments 76
#define MGK_CoderErrorUnableToSetImageTitle 77
#define MGK_CoderErrorUnableToSetJPEGLevel 78
#define MGK_CoderErrorUnableToSetRegionOfInterest 79
#define MGK_CoderErrorUnableToSetSummaryInfo 80
#define MGK_CoderErrorUnableToTranslateText 81
#define MGK_CoderErrorUnableToWriteMPEGParameters 82
#define MGK_CoderErrorUnableToZipCompressImage 83
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 84
#define MGK_CoderErrorZipCompressionNotSupported 85
#define MGK_CoderFatalErrorDefault 86
#define MGK_CoderWarningLosslessToLossyJPEGConversion 87
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 88
#define MGK_ConfigureErrorRegistryKeyLookupFailed 89
#define MGK_ConfigureErrorUnableToAccessConfigureFile 90
#define MGK_ConfigureErrorUnableToAccessFontFile 91
#define MGK_ConfigureErrorUnableToAccessLogFile 92
#define MGK_ConfigureErrorUnableToAccessModuleFile 93
#define MGK_ConfigureFatalErrorDefault 94
#define MGK_ConfigureWarningDefault 95
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 96
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 97
#define MGK_CorruptImageErrorColormapExceeds256Colors 98
#define MGK_CorruptImageErrorCorruptImage 99
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 100
#define MGK_CorruptImageErrorImageTypeNotSupported 101
#define MGK_CorruptImageErrorImproperImageHeader 102
#define MGK_CorruptImageErrorInsufficientImageDataInFile 103
#define MGK_CorruptImageErrorInvalidColormapIndex 104
#define MGK_CorruptImageErrorInvalidFileFormatVersion 105
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 106
#define MGK_CorruptImageErrorMissingImageChannel 107
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 108
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 109
#define MGK_CorruptImageErrorNotEnoughTiles 110
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 111
#define MGK_CorruptImageErrorSubsamplingRequiresEvenWidth 112
#define MGK_CorruptImageErrorTooMuchImageDataInFile 113
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 114
#define MGK_CorruptImageErrorUnableToReadColorProfile 115
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 116
#define MGK_CorruptImageErrorUnableToReadGenericProfile 117
#define MGK_CorruptImageErrorUnableToReadImageData 118
#define MGK_CorruptImageErrorUnableToReadImageHeader 119
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 120
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 121
#define MGK_CorruptImageErrorUnableToReadSubImageData 122
#define MGK_CorruptImageErrorUnableToReadVIDImage 123
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 124
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 125
#define MGK_CorruptImageErrorUnableToUncompressImage 126
#define MGK_CorruptImageErrorUnexpectedEndOfFile 127
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 128
#define MGK_CorruptImageErrorUnknownPatternType 129
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 130
#define MGK_CorruptImageErrorUnrecognizedImageCompression 131
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 132
#define MGK_CorruptImageFatalErrorUnableToPersistKey 133
#define MGK_CorruptImageWarningCompressionNotValid 134
#define MGK_CorruptImageWarningImproperImageHeader 135
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 136
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 137
#define MGK_CorruptImageWarningSkipToSyncByte 138
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 139
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 140
#define MGK_CorruptImageWarningUnrecognizedImageCompression 141
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 142
#define MGK_DelegateErrorDelegateFailed 143
#define MGK_DelegateErrorFailedToComputeOutputSize 144
#define MGK_DelegateErrorFailedToRenderFile 145
#define MGK_DelegateErrorFailedToScanFile 146
#define MGK_DelegateErrorNoTagFound 147
#define MGK_DelegateErrorPostscriptDelegateFailed 148
#define MGK_DelegateErrorUnableToCreateImage 149
#define MGK_DelegateErrorUnableToCreateImageComponent 150
#define MGK_DelegateErrorUnableToDecodeImageFile 151
#define MGK_DelegateErrorUnableToEncodeImageFile 152
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 153
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 154
#define MGK_DelegateErrorUnableToManageJP2Stream 155
#define MGK_DelegateErrorUnableToWriteSVGFormat 156
#define MGK_DelegateFatalErrorDefault 157
#define MGK_DelegateWarningDefault 158
#define MGK_DrawErrorAlreadyPushingPatternDefinition 159
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 160
#define MGK_DrawErrorUnableToPrint 161
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 162
#define MGK_DrawFatalErrorDefault 163
#define MGK_DrawWarningNotARelativeURL 164
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 165
#define MGK_DrawWarningURLNotFound 166
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 167
#define MGK_FileOpenErrorUnableToOpenFile 168
#define MGK_FileOpenErrorUnableToWriteFile 169
#define MGK_FileOpenFatalErrorDefault 170
#define MGK_FileOpenWarningDefault 171
#define MGK_ImageErrorAngleIsDiscontinuous 172
#define MGK_ImageErrorColorspaceColorProfileMismatch 173
#define MGK_ImageErrorImageColorspaceDiffers 174
#define MGK_ImageErrorImageDoesNotContainResolution 175
#define MGK_ImageErrorImageOpacityDiffers 176
#define MGK_ImageErrorImageSequenceIsRequired 177
#define MGK_ImageErrorImageSizeDiffers 178
#define MGK_ImageErrorInvalidColormapIndex 179
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 180
#define MGK_ImageErrorNoImagesWereFound 181
#define MGK_ImageErrorNoImagesWereLoaded 182
#define MGK_ImageErrorNoLocaleImageAttribute 183
#define MGK_ImageErrorTooManyClusters 184
#define MGK_ImageErrorUnableToAppendImage 185
#define MGK_ImageErrorUnableToAssignProfile 186
#define MGK_ImageErrorUnableToAverageImage 187
#define MGK_ImageErrorUnableToCoalesceImage 188
#define MGK_ImageErrorUnableToCompareImages 189
#define MGK_ImageErrorUnableToCreateImageMosaic 190
#define MGK_ImageErrorUnableToCreateStereoImage 191
#define MGK_ImageErrorUnableToDeconstructImageSequence 192
#define MGK_ImageErrorUnableToFlattenImage 193
#define MGK_ImageErrorUnableToGetClipMask 194
#define MGK_ImageErrorUnableToResizeImage 195
#define MGK_ImageErrorUnableToSegmentImage 196
#define MGK_ImageErrorUnableToSetClipMask 197
#define MGK_ImageErrorUnableToShearImage 198
#define MGK_ImageErrorWidthOrHeightExceedsLimit 199
#define MGK_ImageFatalErrorUnableToPersistKey 200
#define MGK_ImageWarningDefault 201
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 202
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 203
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 204
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 205
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 206
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 207
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 208
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 209
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 210
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 211
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 212
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 213
#define MGK_MissingDelegateFatalErrorDefault 214
#define MGK_MissingDelegateWarningDefault 215
#define MGK_ModuleErrorFailedToCloseModule 216
#define MGK_ModuleErrorFailedToFindSymbol 217
#define MGK_ModuleErrorUnableToLoadModule 218
#define MGK_ModuleErrorUnableToRegisterImageFormat 219
#define MGK_ModuleErrorUnrecognizedModule 220
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 221
#define MGK_ModuleWarningDefault 222
#define MGK_OptionErrorBevelWidthIsNegative 223
#define MGK_OptionErrorColorSeparatedImageRequired 224
#define MGK_OptionErrorFrameIsLessThanImageSize 225
#define MGK_OptionErrorGeometryDimensionsAreZero 226
#define MGK_OptionErrorGeometryDoesNotContainImage 227
#define MGK_OptionErrorImagesAreNotTheSameSize 228
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 229
#define MGK_OptionErrorImageSmallerThanKernelWidth 230
#define MGK_OptionErrorImageSmallerThanRadius 231
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 232
#define MGK_OptionErrorInputImagesAlreadySpecified 233
#define MGK_OptionErrorKernelRadiusIsTooSmall 234
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 235
#define MGK_OptionErrorMissingAnImageFilename 236
#define MGK_OptionErrorMissingArgument 237
#define MGK_OptionErrorMustSpecifyAnImageName 238
#define MGK_OptionErrorMustSpecifyImageSize 239
#define MGK_OptionErrorNoBlobDefined 240
#define MGK_OptionErrorNoImagesDefined 241
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 242
#define MGK_OptionErrorNoProfileNameWasGiven 243
#define MGK_OptionErrorNullBlobArgument 244
#define MGK_OptionErrorReferenceImageRequired 245
#define MGK_OptionErrorReferenceIsNotMyType 246
#define MGK_OptionErrorRequestDidNotReturnAnImage 247
#define MGK_OptionErrorSteganoImageRequired 248
#define MGK_OptionErrorStereoImageRequired 249
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 250
#define MGK_OptionErrorUnableToAddOrRemoveProfile 251
#define MGK_OptionErrorUnableToAverageImageSequence 252
#define MGK_OptionErrorUnableToBlurImage 253
#define MGK_OptionErrorUnableToChopImage 254
#define MGK_OptionErrorUnableToConstituteImage 255
#define MGK_OptionErrorUnableToConvolveImage 256
#define MGK_OptionErrorUnableToEdgeImage 257
#define MGK_OptionErrorUnableToEqualizeImage 258
#define MGK_OptionErrorUnableToFilterImage 259
#define MGK_OptionErrorUnableToFormatImageMetadata 260
#define MGK_OptionErrorUnableToFrameImage 261
#define MGK_OptionErrorUnableToOilPaintImage 262
#define MGK_OptionErrorUnableToPaintImage 263
#define MGK_OptionErrorUnableToRaiseImage 264
#define MGK_OptionErrorUnableToSharpenImage 265
#define MGK_OptionErrorUnableToThresholdImage 266
#define MGK_OptionErrorUnableToWaveImage 267
#define MGK_OptionErrorUnrecognizedAttribute 268
#define MGK_OptionErrorUnrecognizedChannelType 269
#define MGK_OptionErrorUnrecognizedColor 270
#define MGK_OptionErrorUnrecognizedColormapType 271
#define MGK_OptionErrorUnrecognizedColorspace 272
#define MGK_OptionErrorUnrecognizedCommand 273
#define MGK_OptionErrorUnrecognizedComposeOperator 274
#define MGK_OptionErrorUnrecognizedDisposeMethod 275
#define MGK_OptionErrorUnrecognizedElement 276
#define MGK_OptionErrorUnrecognizedEndianType 277
#define MGK_OptionErrorUnrecognizedGravityType 278
#define MGK_OptionErrorUnrecognizedImageCompression 279
#define MGK_OptionErrorUnrecognizedImageFilter 280
#define MGK_OptionErrorUnrecognizedImageFormat 281
#define MGK_OptionErrorUnrecognizedImageMode 282
#define MGK_OptionErrorUnrecognizedImageType 283
#define MGK_OptionErrorUnrecognizedIntentType 284
#define MGK_OptionErrorUnrecognizedInterlaceType 285
#define MGK_OptionErrorUnrecognizedListType 286
#define MGK_OptionErrorUnrecognizedModeType 287
#define MGK_OptionErrorUnrecognizedNoiseType 288
#define MGK_OptionErrorUnrecognizedOperator 289
#define MGK_OptionErrorUnrecognizedOption 290
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 291
#define MGK_OptionErrorUnrecognizedPixelMap 292
#define MGK_OptionErrorUnrecognizedPreviewType 293
#define MGK_OptionErrorUnrecognizedResourceType 294
#define MGK_OptionErrorUnrecognizedType 295
#define MGK_OptionErrorUnrecognizedUnitsType 296
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 297
#define MGK_OptionErrorUnsupportedSamplingFactor 298
#define MGK_OptionErrorUsageError 299
#define MGK_OptionFatalErrorInvalidColorspaceType 300
#define MGK_OptionFatalErrorInvalidEndianType 301
#define MGK_OptionFatalErrorInvalidImageType 302
#define MGK_OptionFatalErrorInvalidInterlaceType 303
#define MGK_OptionFatalErrorMissingAnImageFilename 304
#define MGK_OptionFatalErrorMissingArgument 305
#define MGK_OptionFatalErrorNoImagesWereLoaded 306
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 307
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 308
#define MGK_OptionFatalErrorUnableToOpenXServer 309
#define MGK_OptionFatalErrorUnableToPersistKey 310
#define MGK_OptionFatalErrorUnrecognizedColormapType 311
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 312
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 313
#define MGK_OptionFatalErrorUnrecognizedEndianType 314
#define MGK_OptionFatalErrorUnrecognizedFilterType 315
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 316
#define MGK_OptionFatalErrorUnrecognizedImageType 317
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 318
#define MGK_OptionFatalErrorUnrecognizedOption 319
#define MGK_OptionFatalErrorUnrecognizedResourceType 320
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 321
#define MGK_OptionWarningUnrecognizedColor 322
#define MGK_RegistryErrorImageExpected 323
#define MGK_RegistryErrorImageInfoExpected 324
#define MGK_RegistryErrorStructureSizeMismatch 325
#define MGK_RegistryErrorUnableToGetRegistryID 326
#define MGK_RegistryErrorUnableToLocateImage 327
#define MGK_RegistryErrorUnableToSetRegistry 328
#define MGK_RegistryFatalErrorDefault 329
#define MGK_RegistryWarningDefault 330
#define MGK_ResourceLimitErrorCacheResourcesExhausted 331
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 332
#define MGK_ResourceLimitErrorMemoryAllocationFailed 333
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 334
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 335
#define MGK_ResourceLimitErrorUnableToAddColorProfile 336
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 337
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 338
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 339
#define MGK_ResourceLimitErrorUnableToAllocateColormap 340
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 341
#define MGK_ResourceLimitErrorUnableToAllocateString 342
#define MGK_ResourceLimitErrorUnableToAnnotateImage 343
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 344
#define MGK_ResourceLimitErrorUnableToCloneImage 345
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 346
#define MGK_ResourceLimitErrorUnableToConstituteImage 347
#define MGK_ResourceLimitErrorUnableToConvertFont 348
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 349
#define MGK_ResourceLimitErrorUnableToCreateColormap 350
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 351
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 352
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 353
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 354
#define MGK_ResourceLimitErrorUnableToCreateXWindow 355
#define MGK_ResourceLimitErrorUnableToCropImage 356
#define MGK_ResourceLimitErrorUnableToDespeckleImage 357
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 358
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 359
#define MGK_ResourceLimitErrorUnableToDitherImage 360
#define MGK_ResourceLimitErrorUnableToDrawOnImage 361
#define MGK_ResourceLimitErrorUnableToEdgeImage 362
#define MGK_ResourceLimitErrorUnableToEmbossImage 363
#define MGK_ResourceLimitErrorUnableToEnhanceImage 364
#define MGK_ResourceLimitErrorUnableToFloodfillImage 365
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 366
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 367
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 368
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 369
#define MGK_ResourceLimitErrorUnableToLevelImage 370
#define MGK_ResourceLimitErrorUnableToMagnifyImage 371
#define MGK_ResourceLimitErrorUnableToManageColor 372
#define MGK_ResourceLimitErrorUnableToMapImage 373
#define MGK_ResourceLimitErrorUnableToMapImageSequence 374
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 375
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 376
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 377
#define MGK_ResourceLimitErrorUnableToNormalizeImage 378
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 379
#define MGK_ResourceLimitErrorUnableToQuantizeImage 380
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 381
#define MGK_ResourceLimitErrorUnableToReadTextChunk 382
#define MGK_ResourceLimitErrorUnableToReadXImage 383
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 384
#define MGK_ResourceLimitErrorUnableToResizeImage 385
#define MGK_ResourceLimitErrorUnableToRotateImage 386
#define MGK_ResourceLimitErrorUnableToSampleImage 387
#define MGK_ResourceLimitErrorUnableToScaleImage 388
#define MGK_ResourceLimitErrorUnableToSelectImage 389
#define MGK_ResourceLimitErrorUnableToSharpenImage 390
#define MGK_ResourceLimitErrorUnableToShaveImage 391
#define MGK_ResourceLimitErrorUnableToShearImage 392
#define MGK_ResourceLimitErrorUnableToSortImageColormap 393
#define MGK_ResourceLimitErrorUnableToThresholdImage 394
#define MGK_ResourceLimitErrorUnableToTransformColorspace 395
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 396
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 397
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 398
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 399
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 400
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 401
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 402
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 403
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 404
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 405
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 406
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 407
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 408
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 409
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 410
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 411
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 412
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 413
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 414
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 415
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 416
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 417
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 418
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 419
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 420
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 421
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 422
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 423
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 424
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 425
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 426
#define MGK_ResourceLimitFatalErrorUnableToConvertText 427
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 428
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 429
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 430
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 431
#define MGK_ResourceLimitWarningMemoryAllocationFailed 432
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 433
#define MGK_StreamErrorNoStreamHandlerIsDefined 434
#define MGK_StreamErrorPixelCacheIsNotOpen 435
#define MGK_StreamErrorUnableToAcquirePixelStream 436
#define MGK_StreamErrorUnableToSetPixelStream 437
#define MGK_StreamErrorUnableToSyncPixelStream 438
#define MGK_StreamFatalErrorDefault 439
#define MGK_StreamWarningDefault 440
#define MGK_TypeErrorFontSubstitutionRequired 441
#define MGK_TypeErrorUnableToGetTypeMetrics 442
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 443
#define MGK_TypeErrorUnableToReadFont 444
#define MGK_TypeErrorUnrecognizedFontEncoding 445
#define MGK_TypeFatalErrorDefault 446
#define MGK_TypeWarningDefault 447
#define MGK_WandErrorInvalidColormapIndex 448
#define MGK_WandErrorWandAPINotImplemented 449
#define MGK_WandErrorWandContainsNoImageIndexs 450
#define MGK_WandErrorWandContainsNoImages 451
#define MGK_XServerErrorColorIsNotKnownToServer 452
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 453
#define MGK_XServerErrorStandardColormapIsNotInitialized 454
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 455
#define MGK_XServerErrorUnableToCreateBitmap 456
#define MGK_XServerErrorUnableToCreateColormap 457
#define MGK_XServerErrorUnableToCreatePixmap 458
#define MGK_XServerErrorUnableToCreateProperty 459
#define MGK_XServerErrorUnableToCreateStandardColormap 460
#define MGK_XServerErrorUnableToDisplayImageInfo 461
#define MGK_XServerErrorUnableToGetProperty 462
#define MGK_XServerErrorUnableToGetStandardColormap 463
#define MGK_XServerErrorUnableToGetVisual 464
#define MGK_XServerErrorUnableToGrabMouse 465
#define MGK_XServerErrorUnableToLoadFont 466
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 467
#define MGK_XServerErrorUnableToOpenXServer 468
#define MGK_XServerErrorUnableToReadXAttributes 469
#define MGK_XServerErrorUnableToReadXWindowImage 470
#define MGK_XServerErrorUnrecognizedColormapType 471
#define MGK_XServerErrorUnrecognizedGravityType 472
#define MGK_XServerErrorUnrecognizedVisualSpecifier 473
#define MGK_XServerFatalErrorUnableToAllocateXHints 474
#define MGK_XServerFatalErrorUnableToCreateCursor 475
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 476
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 477
#define MGK_XServerFatalErrorUnableToCreateTextProperty 478
#define MGK_XServerFatalErrorUnableToCreateXImage 479
#define MGK_XServerFatalErrorUnableToCreateXPixmap 480
#define MGK_XServerFatalErrorUnableToCreateXWindow 481
#define MGK_XServerFatalErrorUnableToDisplayImage 482
#define MGK_XServerFatalErrorUnableToDitherImage 483
#define MGK_XServerFatalErrorUnableToGetPixelInfo 484
#define MGK_XServerFatalErrorUnableToGetVisual 485
#define MGK_XServerFatalErrorUnableToLoadFont 486
#define MGK_XServerFatalErrorUnableToMakeXWindow 487
#define MGK_XServerFatalErrorUnableToOpenXServer 488
#define MGK_XServerFatalErrorUnableToViewFonts 489
#define MGK_XServerWarningUnableToGetVisual 490
#define MGK_XServerWarningUsingDefaultVisual 491

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
    { "Cache/FatalError", 19, CacheFatalError },
    { "Cache/Warning", 21, CacheWarning },
    { "Coder/Error", 22, CoderError },
    { "Coder/FatalError", 85, CoderFatalError },
    { "Coder/Warning", 86, CoderWarning },
    { "Configure/Error", 87, ConfigureError },
    { "Configure/FatalError", 93, ConfigureFatalError },
    { "Configure/Warning", 94, ConfigureWarning },
    { "Corrupt/Image/Error", 95, CorruptImageError },
    { "Corrupt/Image/FatalError", 132, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 133, CorruptImageWarning },
    { "Delegate/Error", 142, DelegateError },
    { "Delegate/FatalError", 156, DelegateFatalError },
    { "Delegate/Warning", 157, DelegateWarning },
    { "Draw/Error", 158, DrawError },
    { "Draw/FatalError", 162, DrawFatalError },
    { "Draw/Warning", 163, DrawWarning },
    { "File/Open/Error", 166, FileOpenError },
    { "File/Open/FatalError", 169, FileOpenFatalError },
    { "File/Open/Warning", 170, FileOpenWarning },
    { "Image/Error", 171, ImageError },
    { "Image/FatalError", 199, ImageFatalError },
    { "Image/Warning", 200, ImageWarning },
    { "Missing/Delegate/Error", 201, MissingDelegateError },
    { "Missing/Delegate/FatalError", 213, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 214, MissingDelegateWarning },
    { "Module/Error", 215, ModuleError },
    { "Module/FatalError", 220, ModuleFatalError },
    { "Module/Warning", 221, ModuleWarning },
    { "Option/Error", 222, OptionError },
    { "Option/FatalError", 299, OptionFatalError },
    { "Option/Warning", 321, OptionWarning },
    { "Registry/Error", 322, RegistryError },
    { "Registry/FatalError", 328, RegistryFatalError },
    { "Registry/Warning", 329, RegistryWarning },
    { "Resource/Limit/Error", 330, ResourceLimitError },
    { "Resource/Limit/FatalError", 395, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 431, ResourceLimitWarning },
    { "Stream/Error", 432, StreamError },
    { "Stream/FatalError", 438, StreamFatalError },
    { "Stream/Warning", 439, StreamWarning },
    { "Type/Error", 440, TypeError },
    { "Type/FatalError", 445, TypeFatalError },
    { "Type/Warning", 446, TypeWarning },
    { "Wand/Error", 447, WandError },
    { "XServer/Error", 451, XServerError },
    { "XServer/FatalError", 473, XServerFatalError },
    { "XServer/Warning", 489, XServerWarning },
    { 0, 491, UndefinedException }
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
    { "SubsamplingRequiresEvenWidth", 61 },
    { "UnableToCopyProfile", 62 },
    { "UnableToCreateADC", 63 },
    { "UnableToCreateBitmap", 64 },
    { "UnableToDecompressImage", 65 },
    { "UnableToInitializeFPXLibrary", 66 },
    { "UnableToOpenBlob", 67 },
    { "UnableToReadAspectRatio", 68 },
    { "UnableToReadCIELABImages", 69 },
    { "UnableToReadSummaryInfo", 70 },
    { "UnableToSetAffineMatrix", 71 },
    { "UnableToSetAspectRatio", 72 },
    { "UnableToSetColorTwist", 73 },
    { "UnableToSetContrast", 74 },
    { "UnableToSetFilteringValue", 75 },
    { "UnableToSetImageComments", 76 },
    { "UnableToSetImageTitle", 77 },
    { "UnableToSetJPEGLevel", 78 },
    { "UnableToSetRegionOfInterest", 79 },
    { "UnableToSetSummaryInfo", 80 },
    { "UnableToTranslateText", 81 },
    { "UnableToWriteMPEGParameters", 82 },
    { "UnableToZipCompressImage", 83 },
    { "UnsupportedCellTypeInTheMatrix", 84 },
    { "ZipCompressionNotSupported", 85 },
    { "Default", 86 },
    { "LosslessToLossyJPEGConversion", 87 },
    { "IncludeElementNestedTooDeeply", 88 },
    { "RegistryKeyLookupFailed", 89 },
    { "UnableToAccessConfigureFile", 90 },
    { "UnableToAccessFontFile", 91 },
    { "UnableToAccessLogFile", 92 },
    { "UnableToAccessModuleFile", 93 },
    { "Default", 94 },
    { "Default", 95 },
    { "AnErrorHasOccurredReadingFromFile", 96 },
    { "AnErrorHasOccurredWritingToFile", 97 },
    { "ColormapExceeds256Colors", 98 },
    { "CorruptImage", 99 },
    { "ImageFileDoesNotContainAnyImageData", 100 },
    { "ImageTypeNotSupported", 101 },
    { "ImproperImageHeader", 102 },
    { "InsufficientImageDataInFile", 103 },
    { "InvalidColormapIndex", 104 },
    { "InvalidFileFormatVersion", 105 },
    { "LengthAndFilesizeDoNotMatch", 106 },
    { "MissingImageChannel", 107 },
    { "NegativeOrZeroImageSize", 108 },
    { "NonOS2HeaderSizeError", 109 },
    { "NotEnoughTiles", 110 },
    { "StaticPlanesValueNotEqualToOne", 111 },
    { "SubsamplingRequiresEvenWidth", 112 },
    { "TooMuchImageDataInFile", 113 },
    { "UnableToReadColormapFromDumpFile", 114 },
    { "UnableToReadColorProfile", 115 },
    { "UnableToReadExtensionBlock", 116 },
    { "UnableToReadGenericProfile", 117 },
    { "UnableToReadImageData", 118 },
    { "UnableToReadImageHeader", 119 },
    { "UnableToReadIPTCProfile", 120 },
    { "UnableToReadPixmapFromDumpFile", 121 },
    { "UnableToReadSubImageData", 122 },
    { "UnableToReadVIDImage", 123 },
    { "UnableToReadWindowNameFromDumpFile", 124 },
    { "UnableToRunlengthDecodeImage", 125 },
    { "UnableToUncompressImage", 126 },
    { "UnexpectedEndOfFile", 127 },
    { "UnexpectedSamplingFactor", 128 },
    { "UnknownPatternType", 129 },
    { "UnrecognizedBitsPerPixel", 130 },
    { "UnrecognizedImageCompression", 131 },
    { "UnrecognizedXWDHeader", 132 },
    { "UnableToPersistKey", 133 },
    { "CompressionNotValid", 134 },
    { "ImproperImageHeader", 135 },
    { "NegativeOrZeroImageSize", 136 },
    { "NonOS2HeaderSizeError", 137 },
    { "SkipToSyncByte", 138 },
    { "StaticPlanesValueNotEqualToOne", 139 },
    { "UnrecognizedBitsPerPixel", 140 },
    { "UnrecognizedImageCompression", 141 },
    { "UnrecognizedNumberOfColors", 142 },
    { "DelegateFailed", 143 },
    { "FailedToComputeOutputSize", 144 },
    { "FailedToRenderFile", 145 },
    { "FailedToScanFile", 146 },
    { "NoTagFound", 147 },
    { "PostscriptDelegateFailed", 148 },
    { "UnableToCreateImage", 149 },
    { "UnableToCreateImageComponent", 150 },
    { "UnableToDecodeImageFile", 151 },
    { "UnableToEncodeImageFile", 152 },
    { "UnableToInitializeFPXLibrary", 153 },
    { "UnableToInitializeWMFLibrary", 154 },
    { "UnableToManageJP2Stream", 155 },
    { "UnableToWriteSVGFormat", 156 },
    { "Default", 157 },
    { "Default", 158 },
    { "AlreadyPushingPatternDefinition", 159 },
    { "NonconformingDrawingPrimitiveDefinition", 160 },
    { "UnableToPrint", 161 },
    { "UnbalancedGraphicContextPushPop", 162 },
    { "Default", 163 },
    { "NotARelativeURL", 164 },
    { "NotCurrentlyPushingPatternDefinition", 165 },
    { "URLNotFound", 166 },
    { "UnableToCreateTemporaryFile", 167 },
    { "UnableToOpenFile", 168 },
    { "UnableToWriteFile", 169 },
    { "Default", 170 },
    { "Default", 171 },
    { "AngleIsDiscontinuous", 172 },
    { "ColorspaceColorProfileMismatch", 173 },
    { "ImageColorspaceDiffers", 174 },
    { "ImageDoesNotContainResolution", 175 },
    { "ImageOpacityDiffers", 176 },
    { "ImageSequenceIsRequired", 177 },
    { "ImageSizeDiffers", 178 },
    { "InvalidColormapIndex", 179 },
    { "LeftAndRightImageSizesDiffer", 180 },
    { "NoImagesWereFound", 181 },
    { "NoImagesWereLoaded", 182 },
    { "NoLocaleImageAttribute", 183 },
    { "TooManyClusters", 184 },
    { "UnableToAppendImage", 185 },
    { "UnableToAssignProfile", 186 },
    { "UnableToAverageImage", 187 },
    { "UnableToCoalesceImage", 188 },
    { "UnableToCompareImages", 189 },
    { "UnableToCreateImageMosaic", 190 },
    { "UnableToCreateStereoImage", 191 },
    { "UnableToDeconstructImageSequence", 192 },
    { "UnableToFlattenImage", 193 },
    { "UnableToGetClipMask", 194 },
    { "UnableToResizeImage", 195 },
    { "UnableToSegmentImage", 196 },
    { "UnableToSetClipMask", 197 },
    { "UnableToShearImage", 198 },
    { "WidthOrHeightExceedsLimit", 199 },
    { "UnableToPersistKey", 200 },
    { "Default", 201 },
    { "DPSLibraryIsNotAvailable", 202 },
    { "FPXLibraryIsNotAvailable", 203 },
    { "FreeTypeLibraryIsNotAvailable", 204 },
    { "JPEGLibraryIsNotAvailable", 205 },
    { "LCMSLibraryIsNotAvailable", 206 },
    { "LZWEncodingNotEnabled", 207 },
    { "NoDecodeDelegateForThisImageFormat", 208 },
    { "NoEncodeDelegateForThisImageFormat", 209 },
    { "TIFFLibraryIsNotAvailable", 210 },
    { "XMLLibraryIsNotAvailable", 211 },
    { "XWindowLibraryIsNotAvailable", 212 },
    { "ZipLibraryIsNotAvailable", 213 },
    { "Default", 214 },
    { "Default", 215 },
    { "FailedToCloseModule", 216 },
    { "FailedToFindSymbol", 217 },
    { "UnableToLoadModule", 218 },
    { "UnableToRegisterImageFormat", 219 },
    { "UnrecognizedModule", 220 },
    { "UnableToInitializeModuleLoader", 221 },
    { "Default", 222 },
    { "BevelWidthIsNegative", 223 },
    { "ColorSeparatedImageRequired", 224 },
    { "FrameIsLessThanImageSize", 225 },
    { "GeometryDimensionsAreZero", 226 },
    { "GeometryDoesNotContainImage", 227 },
    { "ImagesAreNotTheSameSize", 228 },
    { "ImageSizeMustExceedBevelWidth", 229 },
    { "ImageSmallerThanKernelWidth", 230 },
    { "ImageSmallerThanRadius", 231 },
    { "ImageWidthsOrHeightsDiffer", 232 },
    { "InputImagesAlreadySpecified", 233 },
    { "KernelRadiusIsTooSmall", 234 },
    { "KernelWidthMustBeAnOddNumber", 235 },
    { "MissingAnImageFilename", 236 },
    { "MissingArgument", 237 },
    { "MustSpecifyAnImageName", 238 },
    { "MustSpecifyImageSize", 239 },
    { "NoBlobDefined", 240 },
    { "NoImagesDefined", 241 },
    { "NonzeroWidthAndHeightRequired", 242 },
    { "NoProfileNameWasGiven", 243 },
    { "NullBlobArgument", 244 },
    { "ReferenceImageRequired", 245 },
    { "ReferenceIsNotMyType", 246 },
    { "RequestDidNotReturnAnImage", 247 },
    { "SteganoImageRequired", 248 },
    { "StereoImageRequired", 249 },
    { "SubimageSpecificationReturnsNoImages", 250 },
    { "UnableToAddOrRemoveProfile", 251 },
    { "UnableToAverageImageSequence", 252 },
    { "UnableToBlurImage", 253 },
    { "UnableToChopImage", 254 },
    { "UnableToConstituteImage", 255 },
    { "UnableToConvolveImage", 256 },
    { "UnableToEdgeImage", 257 },
    { "UnableToEqualizeImage", 258 },
    { "UnableToFilterImage", 259 },
    { "UnableToFormatImageMetadata", 260 },
    { "UnableToFrameImage", 261 },
    { "UnableToOilPaintImage", 262 },
    { "UnableToPaintImage", 263 },
    { "UnableToRaiseImage", 264 },
    { "UnableToSharpenImage", 265 },
    { "UnableToThresholdImage", 266 },
    { "UnableToWaveImage", 267 },
    { "UnrecognizedAttribute", 268 },
    { "UnrecognizedChannelType", 269 },
    { "UnrecognizedColor", 270 },
    { "UnrecognizedColormapType", 271 },
    { "UnrecognizedColorspace", 272 },
    { "UnrecognizedCommand", 273 },
    { "UnrecognizedComposeOperator", 274 },
    { "UnrecognizedDisposeMethod", 275 },
    { "UnrecognizedElement", 276 },
    { "UnrecognizedEndianType", 277 },
    { "UnrecognizedGravityType", 278 },
    { "UnrecognizedImageCompression", 279 },
    { "UnrecognizedImageFilter", 280 },
    { "UnrecognizedImageFormat", 281 },
    { "UnrecognizedImageMode", 282 },
    { "UnrecognizedImageType", 283 },
    { "UnrecognizedIntentType", 284 },
    { "UnrecognizedInterlaceType", 285 },
    { "UnrecognizedListType", 286 },
    { "UnrecognizedModeType", 287 },
    { "UnrecognizedNoiseType", 288 },
    { "UnrecognizedOperator", 289 },
    { "UnrecognizedOption", 290 },
    { "UnrecognizedPerlMagickMethod", 291 },
    { "UnrecognizedPixelMap", 292 },
    { "UnrecognizedPreviewType", 293 },
    { "UnrecognizedResourceType", 294 },
    { "UnrecognizedType", 295 },
    { "UnrecognizedUnitsType", 296 },
    { "UnrecognizedVirtualPixelMethod", 297 },
    { "UnsupportedSamplingFactor", 298 },
    { "UsageError", 299 },
    { "InvalidColorspaceType", 300 },
    { "InvalidEndianType", 301 },
    { "InvalidImageType", 302 },
    { "InvalidInterlaceType", 303 },
    { "MissingAnImageFilename", 304 },
    { "MissingArgument", 305 },
    { "NoImagesWereLoaded", 306 },
    { "OptionLengthExceedsLimit", 307 },
    { "RequestDidNotReturnAnImage", 308 },
    { "UnableToOpenXServer", 309 },
    { "UnableToPersistKey", 310 },
    { "UnrecognizedColormapType", 311 },
    { "UnrecognizedColorspaceType", 312 },
    { "UnrecognizedDisposeMethod", 313 },
    { "UnrecognizedEndianType", 314 },
    { "UnrecognizedFilterType", 315 },
    { "UnrecognizedImageCompressionType", 316 },
    { "UnrecognizedImageType", 317 },
    { "UnrecognizedInterlaceType", 318 },
    { "UnrecognizedOption", 319 },
    { "UnrecognizedResourceType", 320 },
    { "UnrecognizedVirtualPixelMethod", 321 },
    { "UnrecognizedColor", 322 },
    { "ImageExpected", 323 },
    { "ImageInfoExpected", 324 },
    { "StructureSizeMismatch", 325 },
    { "UnableToGetRegistryID", 326 },
    { "UnableToLocateImage", 327 },
    { "UnableToSetRegistry", 328 },
    { "Default", 329 },
    { "Default", 330 },
    { "CacheResourcesExhausted", 331 },
    { "ImagePixelLimitExceeded", 332 },
    { "MemoryAllocationFailed", 333 },
    { "NoPixelsDefinedInCache", 334 },
    { "PixelCacheAllocationFailed", 335 },
    { "UnableToAddColorProfile", 336 },
    { "UnableToAddGenericProfile", 337 },
    { "UnableToAddIPTCProfile", 338 },
    { "UnableToAllocateCoefficients", 339 },
    { "UnableToAllocateColormap", 340 },
    { "UnableToAllocateICCProfile", 341 },
    { "UnableToAllocateString", 342 },
    { "UnableToAnnotateImage", 343 },
    { "UnableToAverageImageSequence", 344 },
    { "UnableToCloneImage", 345 },
    { "UnableToComputeImageSignature", 346 },
    { "UnableToConstituteImage", 347 },
    { "UnableToConvertFont", 348 },
    { "UnableToConvertStringToTokens", 349 },
    { "UnableToCreateColormap", 350 },
    { "UnableToCreateColorTransform", 351 },
    { "UnableToCreateCommandWidget", 352 },
    { "UnableToCreateImageGroup", 353 },
    { "UnableToCreateImageMontage", 354 },
    { "UnableToCreateXWindow", 355 },
    { "UnableToCropImage", 356 },
    { "UnableToDespeckleImage", 357 },
    { "UnableToDetermineImageClass", 358 },
    { "UnableToDetermineTheNumberOfImageColors", 359 },
    { "UnableToDitherImage", 360 },
    { "UnableToDrawOnImage", 361 },
    { "UnableToEdgeImage", 362 },
    { "UnableToEmbossImage", 363 },
    { "UnableToEnhanceImage", 364 },
    { "UnableToFloodfillImage", 365 },
    { "UnableToGammaCorrectImage", 366 },
    { "UnableToGetBestIconSize", 367 },
    { "UnableToGetFromRegistry", 368 },
    { "UnableToGetPackageInfo", 369 },
    { "UnableToLevelImage", 370 },
    { "UnableToMagnifyImage", 371 },
    { "UnableToManageColor", 372 },
    { "UnableToMapImage", 373 },
    { "UnableToMapImageSequence", 374 },
    { "UnableToMedianFilterImage", 375 },
    { "UnableToMotionBlurImage", 376 },
    { "UnableToNoiseFilterImage", 377 },
    { "UnableToNormalizeImage", 378 },
    { "UnableToOpenColorProfile", 379 },
    { "UnableToQuantizeImage", 380 },
    { "UnableToQuantizeImageSequence", 381 },
    { "UnableToReadTextChunk", 382 },
    { "UnableToReadXImage", 383 },
    { "UnableToReadXServerColormap", 384 },
    { "UnableToResizeImage", 385 },
    { "UnableToRotateImage", 386 },
    { "UnableToSampleImage", 387 },
    { "UnableToScaleImage", 388 },
    { "UnableToSelectImage", 389 },
    { "UnableToSharpenImage", 390 },
    { "UnableToShaveImage", 391 },
    { "UnableToShearImage", 392 },
    { "UnableToSortImageColormap", 393 },
    { "UnableToThresholdImage", 394 },
    { "UnableToTransformColorspace", 395 },
    { "MemoryAllocationFailed", 396 },
    { "UnableToAllocateAscii85Info", 397 },
    { "UnableToAllocateCacheInfo", 398 },
    { "UnableToAllocateCacheView", 399 },
    { "UnableToAllocateColorInfo", 400 },
    { "UnableToAllocateDashPattern", 401 },
    { "UnableToAllocateDelegateInfo", 402 },
    { "UnableToAllocateDerivatives", 403 },
    { "UnableToAllocateDrawContext", 404 },
    { "UnableToAllocateDrawInfo", 405 },
    { "UnableToAllocateDrawingWand", 406 },
    { "UnableToAllocateGammaMap", 407 },
    { "UnableToAllocateImage", 408 },
    { "UnableToAllocateImagePixels", 409 },
    { "UnableToAllocateLogInfo", 410 },
    { "UnableToAllocateMagicInfo", 411 },
    { "UnableToAllocateMagickInfo", 412 },
    { "UnableToAllocateModuleInfo", 413 },
    { "UnableToAllocateMontageInfo", 414 },
    { "UnableToAllocateQuantizeInfo", 415 },
    { "UnableToAllocateRegistryInfo", 416 },
    { "UnableToAllocateSemaphoreInfo", 417 },
    { "UnableToAllocateString", 418 },
    { "UnableToAllocateTypeInfo", 419 },
    { "UnableToAllocateWand", 420 },
    { "UnableToAnimateImageSequence", 421 },
    { "UnableToCloneBlobInfo", 422 },
    { "UnableToCloneCacheInfo", 423 },
    { "UnableToCloneImage", 424 },
    { "UnableToCloneImageInfo", 425 },
    { "UnableToConcatenateString", 426 },
    { "UnableToConvertText", 427 },
    { "UnableToCreateColormap", 428 },
    { "UnableToDisplayImage", 429 },
    { "UnableToEscapeString", 430 },
    { "UnableToInterpretMSLImage", 431 },
    { "MemoryAllocationFailed", 432 },
    { "ImageDoesNotContainTheStreamGeometry", 433 },
    { "NoStreamHandlerIsDefined", 434 },
    { "PixelCacheIsNotOpen", 435 },
    { "UnableToAcquirePixelStream", 436 },
    { "UnableToSetPixelStream", 437 },
    { "UnableToSyncPixelStream", 438 },
    { "Default", 439 },
    { "Default", 440 },
    { "FontSubstitutionRequired", 441 },
    { "UnableToGetTypeMetrics", 442 },
    { "UnableToInitializeFreetypeLibrary", 443 },
    { "UnableToReadFont", 444 },
    { "UnrecognizedFontEncoding", 445 },
    { "Default", 446 },
    { "Default", 447 },
    { "InvalidColormapIndex", 448 },
    { "WandAPINotImplemented", 449 },
    { "WandContainsNoImageIndexs", 450 },
    { "WandContainsNoImages", 451 },
    { "ColorIsNotKnownToServer", 452 },
    { "NoWindowWithSpecifiedIDExists", 453 },
    { "StandardColormapIsNotInitialized", 454 },
    { "UnableToConnectToRemoteDisplay", 455 },
    { "UnableToCreateBitmap", 456 },
    { "UnableToCreateColormap", 457 },
    { "UnableToCreatePixmap", 458 },
    { "UnableToCreateProperty", 459 },
    { "UnableToCreateStandardColormap", 460 },
    { "UnableToDisplayImageInfo", 461 },
    { "UnableToGetProperty", 462 },
    { "UnableToGetStandardColormap", 463 },
    { "UnableToGetVisual", 464 },
    { "UnableToGrabMouse", 465 },
    { "UnableToLoadFont", 466 },
    { "UnableToMatchVisualToStandardColormap", 467 },
    { "UnableToOpenXServer", 468 },
    { "UnableToReadXAttributes", 469 },
    { "UnableToReadXWindowImage", 470 },
    { "UnrecognizedColormapType", 471 },
    { "UnrecognizedGravityType", 472 },
    { "UnrecognizedVisualSpecifier", 473 },
    { "UnableToAllocateXHints", 474 },
    { "UnableToCreateCursor", 475 },
    { "UnableToCreateGraphicContext", 476 },
    { "UnableToCreateStandardColormap", 477 },
    { "UnableToCreateTextProperty", 478 },
    { "UnableToCreateXImage", 479 },
    { "UnableToCreateXPixmap", 480 },
    { "UnableToCreateXWindow", 481 },
    { "UnableToDisplayImage", 482 },
    { "UnableToDitherImage", 483 },
    { "UnableToGetPixelInfo", 484 },
    { "UnableToGetVisual", 485 },
    { "UnableToLoadFont", 486 },
    { "UnableToMakeXWindow", 487 },
    { "UnableToOpenXServer", 488 },
    { "UnableToViewFonts", 489 },
    { "UnableToGetVisual", 490 },
    { "UsingDefaultVisual", 491 },
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
    "Request did not return an image",
    "Stegano image required",
    "Stereo image required",
    "Subimage specification returns no images",
    "Unable to add or remove profile",
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
