#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 611

#define BlobErrorUnableToCreateBlob 1
#define BlobErrorUnableToOpenFile 2
#define BlobErrorUnableToReadFile 3
#define BlobErrorUnableToReadToOffset 4
#define BlobErrorUnableToSeekToOffset 5
#define BlobErrorUnableToWriteBlob 6
#define BlobErrorUnrecognizedImageFormat 7
#define BlobFatalErrorDefault 8
#define BlobWarningDefault 9
#define CacheErrorInconsistentPersistentCacheDepth 10
#define CacheErrorPixelCacheIsNotOpen 11
#define CacheErrorUnableToCloneCache 12
#define CacheErrorUnableToExtendCache 13
#define CacheErrorUnableToGetCacheNexus 14
#define CacheErrorUnableToGetPixelsFromCache 15
#define CacheErrorUnableToOpenCache 16
#define CacheErrorUnableToPeristPixelCache 17
#define CacheErrorUnableToReadPixelCache 18
#define CacheErrorUnableToSyncCache 19
#define CacheFatalErrorDiskAllocationFailed 20
#define CacheFatalErrorUnableToExtendPixelCache 21
#define CacheWarningDefault 22
#define CoderErrorColormapTypeNotSupported 23
#define CoderErrorColorspaceModelIsNotSupported 24
#define CoderErrorColorTypeNotSupported 25
#define CoderErrorCompressionNotValid 26
#define CoderErrorDataEncodingSchemeIsNotSupported 27
#define CoderErrorDataStorageTypeIsNotSupported 28
#define CoderErrorDeltaPNGNotSupported 29
#define CoderErrorEncryptedWPGImageFileNotSupported 30
#define CoderErrorFractalCompressionNotSupported 31
#define CoderErrorImageColumnOrRowSizeIsNotSupported 32
#define CoderErrorImageDoesNotHaveAMatteChannel 33
#define CoderErrorImageIsNotTiled 34
#define CoderErrorImageTypeNotSupported 35
#define CoderErrorIncompatibleSizeOfDouble 36
#define CoderErrorIrregularChannelGeometryNotSupported 37
#define CoderErrorJPEGCompressionNotSupported 38
#define CoderErrorJPEGEmbeddingFailed 39
#define CoderErrorLocationTypeIsNotSupported 40
#define CoderErrorMapStorageTypeIsNotSupported 41
#define CoderErrorMSBByteOrderNotSupported 42
#define CoderErrorMultidimensionalMatricesAreNotSupported 43
#define CoderErrorMultipleRecordListNotSupported 44
#define CoderErrorNo8BIMDataIsAvailable 45
#define CoderErrorNoAPP1DataIsAvailable 46
#define CoderErrorNoBitmapOnClipboard 47
#define CoderErrorNoColorProfileAvailable 48
#define CoderErrorNoDataReturned 49
#define CoderErrorNoImageVectorGraphics 50
#define CoderErrorNoIPTCInfoWasFound 51
#define CoderErrorNoIPTCProfileAvailable 52
#define CoderErrorNotASupportedImageFile 53
#define CoderErrorNumberOfImagesIsNotSupported 54
#define CoderErrorOnlyContinuousTonePictureSupported 55
#define CoderErrorOnlyLevelZerofilesSupported 56
#define CoderErrorPNGCompressionNotSupported 57
#define CoderErrorRLECompressionNotSupported 58
#define CoderErrorUnableToCopyProfile 59
#define CoderErrorUnableToCreateADC 60
#define CoderErrorUnableToCreateBitmap 61
#define CoderErrorUnableToDecompressImage 62
#define CoderErrorUnableToInitializeFPXLibrary 63
#define CoderErrorUnableToOpenBlob 64
#define CoderErrorUnableToReadAspectRatio 65
#define CoderErrorUnableToReadCIELABImages 66
#define CoderErrorUnableToReadSummaryInfo 67
#define CoderErrorUnableToSetAffineMatrix 68
#define CoderErrorUnableToSetAspectRatio 69
#define CoderErrorUnableToSetColorTwist 70
#define CoderErrorUnableToSetContrast 71
#define CoderErrorUnableToSetFilteringValue 72
#define CoderErrorUnableToSetImageComments 73
#define CoderErrorUnableToSetImageTitle 74
#define CoderErrorUnableToSetJPEGLevel 75
#define CoderErrorUnableToSetRegionOfInterest 76
#define CoderErrorUnableToSetSummaryInfo 77
#define CoderErrorUnableToTranslateText 78
#define CoderErrorUnableToWriteMPEGParameters 79
#define CoderErrorUnableToZipCompressImage 80
#define CoderErrorUnsupportedCellTypeInTheMatrix 81
#define CoderErrorZipCompressionNotSupported 82
#define CoderFatalErrorDefault 83
#define CoderWarningLosslessToLossyJPEGConversion 84
#define ConfigureErrorIncludeElementNestedTooDeeply 85
#define ConfigureErrorRegistryKeyLookupFailed 86
#define ConfigureErrorUnableToAccessConfigureFile 87
#define ConfigureErrorUnableToAccessFontFile 88
#define ConfigureErrorUnableToAccessLogFile 89
#define ConfigureErrorUnableToAccessModuleFile 90
#define ConfigureFatalErrorDefault 91
#define ConfigureWarningDefault 92
#define CorruptImageErrorAnErrorHasOccurredReadingFromFile 93
#define CorruptImageErrorAnErrorHasOccurredWritingToFile 94
#define CorruptImageErrorColormapExceeds256Colors 95
#define CorruptImageErrorCorruptImage 96
#define CorruptImageErrorImageFileDoesNotContainAnyImageData 97
#define CorruptImageErrorImageTypeNotSupported 98
#define CorruptImageErrorImproperImageHeader 99
#define CorruptImageErrorInsufficientImageDataInFile 100
#define CorruptImageErrorInvalidColormapIndex 101
#define CorruptImageErrorInvalidFileFormatVersion 102
#define CorruptImageErrorLengthAndFilesizeDoNotMatch 103
#define CorruptImageErrorMissingImageChannel 104
#define CorruptImageErrorNegativeOrZeroImageSize 105
#define CorruptImageErrorNonOS2HeaderSizeError 106
#define CorruptImageErrorNotABMPImageFile 107
#define CorruptImageErrorNotACUTImageFile 108
#define CorruptImageErrorNotADCMImageFile 109
#define CorruptImageErrorNotADCXImageFile 110
#define CorruptImageErrorNotADIBImageFile 111
#define CorruptImageErrorNotADPXImageFile 112
#define CorruptImageErrorNotAGIFImageFile 113
#define CorruptImageErrorNotAJNGImageFile 114
#define CorruptImageErrorNotAMATLABImageFile 115
#define CorruptImageErrorNotAMNGImageFile 116
#define CorruptImageErrorNotAMTVImageFile 117
#define CorruptImageErrorNotAnARTImageFile 118
#define CorruptImageErrorNotAnAVIImageFile 119
#define CorruptImageErrorNotAnAVSImageFile 120
#define CorruptImageErrorNotAnEMFImageFile 121
#define CorruptImageErrorNotAnICOImageFile 122
#define CorruptImageErrorNotAOTBImageFile 123
#define CorruptImageErrorNotAPCDImageFile 124
#define CorruptImageErrorNotAPCXImageFile 125
#define CorruptImageErrorNotAPDBImageFile 126
#define CorruptImageErrorNotAPICTImageFile 127
#define CorruptImageErrorNotAPIXImageFile 128
#define CorruptImageErrorNotAPNGImageFile 129
#define CorruptImageErrorNotAPNMImageFile 130
#define CorruptImageErrorNotAPSDImageFile 131
#define CorruptImageErrorNotAPWPImageFile 132
#define CorruptImageErrorNotARLEImageFile 133
#define CorruptImageErrorNotASCTImageFile 134
#define CorruptImageErrorNotASFWImageFile 135
#define CorruptImageErrorNotASGIImageFile 136
#define CorruptImageErrorNotASUNImageFile 137
#define CorruptImageErrorNotATGAImageFile 138
#define CorruptImageErrorNotATIMImageFile 139
#define CorruptImageErrorNotATOPOLImageFile 140
#define CorruptImageErrorNotAVIFFImageFile 141
#define CorruptImageErrorNotAWBMPImageFile 142
#define CorruptImageErrorNotAWPGImageFile 143
#define CorruptImageErrorNotAXBMImageFile 144
#define CorruptImageErrorNotAXCFImageFile 145
#define CorruptImageErrorNotAXPMImageFile 146
#define CorruptImageErrorNotEnoughTiles 147
#define CorruptImageErrorStaticPlanesValueNotEqualToOne 148
#define CorruptImageErrorTooMuchImageDataInFile 149
#define CorruptImageErrorUnableToReadColormapFromDumpFile 150
#define CorruptImageErrorUnableToReadColorProfile 151
#define CorruptImageErrorUnableToReadExtensionBlock 152
#define CorruptImageErrorUnableToReadGenericProfile 153
#define CorruptImageErrorUnableToReadImageData 154
#define CorruptImageErrorUnableToReadImageHeader 155
#define CorruptImageErrorUnableToReadIPTCProfile 156
#define CorruptImageErrorUnableToReadPixmapFromDumpFile 157
#define CorruptImageErrorUnableToReadSubImageData 158
#define CorruptImageErrorUnableToReadVIDImage 159
#define CorruptImageErrorUnableToReadWindowNameFromDumpFile 160
#define CorruptImageErrorUnableToRunlengthDecodeImage 161
#define CorruptImageErrorUnableToUncompressImage 162
#define CorruptImageErrorUnexpectedEndOfFile 163
#define CorruptImageErrorUnexpectedSamplingFactor 164
#define CorruptImageErrorUnknownPatternType 165
#define CorruptImageErrorUnrecognizedBitsPerPixel 166
#define CorruptImageErrorUnrecognizedImageCompression 167
#define CorruptImageErrorUnrecognizedXWDHeader 168
#define CorruptImageFatalErrorUnableToPersistKey 169
#define CorruptImageWarningCompressionNotValid 170
#define CorruptImageWarningImproperImageHeader 171
#define CorruptImageWarningNegativeOrZeroImageSize 172
#define CorruptImageWarningNonOS2HeaderSizeError 173
#define CorruptImageWarningSkipToSyncByte 174
#define CorruptImageWarningStaticPlanesValueNotEqualToOne 175
#define CorruptImageWarningUnrecognizedBitsPerPixel 176
#define CorruptImageWarningUnrecognizedImageCompression 177
#define CorruptImageWarningUnrecognizedNumberOfColors 178
#define DelegateErrorDelegateFailed 179
#define DelegateErrorFailedToComputeOutputSize 180
#define DelegateErrorFailedToRenderFile 181
#define DelegateErrorFailedToScanFile 182
#define DelegateErrorNoTagFound 183
#define DelegateErrorPostscriptDelegateFailed 184
#define DelegateErrorUnableToCreateImage 185
#define DelegateErrorUnableToCreateImageComponent 186
#define DelegateErrorUnableToDecodeImageFile 187
#define DelegateErrorUnableToEncodeImageFile 188
#define DelegateErrorUnableToInitializeFPXLibrary 189
#define DelegateErrorUnableToInitializeWMFLibrary 190
#define DelegateErrorUnableToManageJP2Stream 191
#define DelegateErrorUnableToWriteSVGFormat 192
#define DelegateFatalErrorDefault 193
#define DelegateWarningDefault 194
#define DrawErrorAlreadyPushingPatternDefinition 195
#define DrawErrorNonconformingDrawingPrimitiveDefinition 196
#define DrawErrorUnableToPrint 197
#define DrawErrorUnbalancedGraphicContextPushPop 198
#define DrawFatalErrorDefault 199
#define DrawWarningNotARelativeURL 200
#define DrawWarningNotCurrentlyPushingPatternDefinition 201
#define DrawWarningURLNotFound 202
#define FileOpenErrorUnableToCreateTemporaryFile 203
#define FileOpenErrorUnableToOpenFile 204
#define FileOpenErrorUnableToWriteFile 205
#define FileOpenFatalErrorDefault 206
#define FileOpenWarningDefault 207
#define ImageErrorAngleIsDiscontinuous 208
#define ImageErrorImageColorspaceDiffers 209
#define ImageErrorImageOpacityDiffers 210
#define ImageErrorImageSequenceIsRequired 211
#define ImageErrorImageSizeDiffers 212
#define ImageErrorLeftAndRightImageSizesDiffer 213
#define ImageErrorNoImagesWereFound 214
#define ImageErrorNoImagesWereLoaded 215
#define ImageErrorNoLocaleImageAttribute 216
#define ImageErrorTooManyClusters 217
#define ImageErrorUnableToAppendImage 218
#define ImageErrorUnableToAverageImage 219
#define ImageErrorUnableToCoalesceImage 220
#define ImageErrorUnableToCompareImages 221
#define ImageErrorUnableToCreateImageMosaic 222
#define ImageErrorUnableToCreateStereoImage 223
#define ImageErrorUnableToDeconstructImageSequence 224
#define ImageErrorUnableToFlattenImage 225
#define ImageErrorUnableToResizeImage 226
#define ImageErrorUnableToSegmentImage 227
#define ImageErrorUnableToSetClipMask 228
#define ImageErrorUnableToShearImage 229
#define ImageErrorWidthOrHeightExceedsLimit 230
#define ImageFatalErrorUnableToPersistKey 231
#define ImageWarningDefault 232
#define MissingDelegateErrorDPSLibraryIsNotAvailable 233
#define MissingDelegateErrorFPXLibraryIsNotAvailable 234
#define MissingDelegateErrorFreeTypeLibraryIsNotAvailable 235
#define MissingDelegateErrorJPEGLibraryIsNotAvailable 236
#define MissingDelegateErrorLZWEncodingNotEnabled 237
#define MissingDelegateErrorNoDecodeDelegateForThisImageFormat 238
#define MissingDelegateErrorNoEncodeDelegateForThisImageFormat 239
#define MissingDelegateErrorTIFFLibraryIsNotAvailable 240
#define MissingDelegateErrorXMLLibraryIsNotAvailable 241
#define MissingDelegateErrorXWindowLibraryIsNotAvailable 242
#define MissingDelegateErrorZipLibraryIsNotAvailable 243
#define MissingDelegateFatalErrorDefault 244
#define MissingDelegateWarningDefault 245
#define ModuleErrorFailedToCloseModule 246
#define ModuleErrorFailedToFindSymbol 247
#define ModuleErrorUnableToLoadModule 248
#define ModuleErrorUnableToRegisterImageFormat 249
#define ModuleErrorUnrecognizedModule 250
#define ModuleFatalErrorUnableToInitializeModuleLoader 251
#define ModuleWarningDefault 252
#define OptionErrorBevelWidthIsNegative 253
#define OptionErrorColorSeparatedImageRequired 254
#define OptionErrorFrameIsLessThanImageSize 255
#define OptionErrorGeometryDimensionsAreZero 256
#define OptionErrorGeometryDoesNotContainImage 257
#define OptionErrorImagesAreNotTheSameSize 258
#define OptionErrorImageSizeMustExceedBevelWidth 259
#define OptionErrorImageSmallerThanKernelWidth 260
#define OptionErrorImageSmallerThanRadius 261
#define OptionErrorImageWidthsOrHeightsDiffer 262
#define OptionErrorInputImagesAlreadySpecified 263
#define OptionErrorKernelRadiusIsTooSmall 264
#define OptionErrorKernelWidthMustBeAnOddNumber 265
#define OptionErrorMissingAffineMatrix 266
#define OptionErrorMissingAmount 267
#define OptionErrorMissingAmplitude 268
#define OptionErrorMissingAnImageFilename 269
#define OptionErrorMissingArgument 270
#define OptionErrorMissingAzimuth 271
#define OptionErrorMissingBackgroundColor 272
#define OptionErrorMissingBevelWidth 273
#define OptionErrorMissingBorderColor 274
#define OptionErrorMissingCoefficients 275
#define OptionErrorMissingColorizeValue 276
#define OptionErrorMissingColors 277
#define OptionErrorMissingComment 278
#define OptionErrorMissingCompressionQuality 279
#define OptionErrorMissingCycleAmount 280
#define OptionErrorMissingDelay 281
#define OptionErrorMissingDisposeMethod 282
#define OptionErrorMissingDissolveValue 283
#define OptionErrorMissingDistance 284
#define OptionErrorMissingDrawingPrimitive 285
#define OptionErrorMissingEnodingType 286
#define OptionErrorMissingEventMask 287
#define OptionErrorMissingFilename 288
#define OptionErrorMissingFillColor 289
#define OptionErrorMissingFontName 290
#define OptionErrorMissingFormatSpecifier 291
#define OptionErrorMissingGeometry 292
#define OptionErrorMissingImageDepth 293
#define OptionErrorMissingImageFrames 294
#define OptionErrorMissingImageGamma 295
#define OptionErrorMissingImageIterations 296
#define OptionErrorMissingImageLabel 297
#define OptionErrorMissingImageProfile 298
#define OptionErrorMissingImageRotation 299
#define OptionErrorMissingImageScene 300
#define OptionErrorMissingImageType 301
#define OptionErrorMissingImplodeAmount 302
#define OptionErrorMissingListName 303
#define OptionErrorMissingLogFormat 304
#define OptionErrorMissingMatrix 305
#define OptionErrorMissingMatteColor 306
#define OptionErrorMissingOffset 307
#define OptionErrorMissingOpaqueColor 308
#define OptionErrorMissingOrder 309
#define OptionErrorMissingPageGeometry 310
#define OptionErrorMissingPaintRadius 311
#define OptionErrorMissingPassword 312
#define OptionErrorMissingPoint 313
#define OptionErrorMissingPointsize 314
#define OptionErrorMissingResourceLimit 315
#define OptionErrorMissingResourceType 316
#define OptionErrorMissingSeedValue 317
#define OptionErrorMissingServerName 318
#define OptionErrorMissingSpreadAmount 319
#define OptionErrorMissingThreshold 320
#define OptionErrorMissingTile 321
#define OptionErrorMissingTitle 322
#define OptionErrorMissingTransparentColor 323
#define OptionErrorMissingTreeDepth 324
#define OptionErrorMissingType 325
#define OptionErrorMissingViewTransform 326
#define OptionErrorMissingVirtualPixelMethod 327
#define OptionErrorMissingWidth 328
#define OptionErrorMustSpecifyAnImageName 329
#define OptionErrorMustSpecifyImageSize 330
#define OptionErrorNoBlobDefined 331
#define OptionErrorNoImagesDefined 332
#define OptionErrorNonzeroWidthAndHeightRequired 333
#define OptionErrorNoProfileNameWasGiven 334
#define OptionErrorNullBlobArgument 335
#define OptionErrorReferenceImageRequired 336
#define OptionErrorReferenceIsNotMyType 337
#define OptionErrorSteganoImageRequired 338
#define OptionErrorStereoImageRequired 339
#define OptionErrorSubimageSpecificationReturnsNoImages 340
#define OptionErrorUnableToAddOrRemoveProfile 341
#define OptionErrorUnableToAllocateICCProfile 342
#define OptionErrorUnableToAverageImageSequence 343
#define OptionErrorUnableToBlurImage 344
#define OptionErrorUnableToChopImage 345
#define OptionErrorUnableToConstituteImage 346
#define OptionErrorUnableToConvolveImage 347
#define OptionErrorUnableToEdgeImage 348
#define OptionErrorUnableToEqualizeImage 349
#define OptionErrorUnableToFilterImage 350
#define OptionErrorUnableToFormatImageMetadata 351
#define OptionErrorUnableToFrameImage 352
#define OptionErrorUnableToOilPaintImage 353
#define OptionErrorUnableToPaintImage 354
#define OptionErrorUnableToRaiseImage 355
#define OptionErrorUnableToSharpenImage 356
#define OptionErrorUnableToThresholdImage 357
#define OptionErrorUnableToWaveImage 358
#define OptionErrorUnrecognizedAttribute 359
#define OptionErrorUnrecognizedChannelType 360
#define OptionErrorUnrecognizedColor 361
#define OptionErrorUnrecognizedColormapType 362
#define OptionErrorUnrecognizedColorspace 363
#define OptionErrorUnrecognizedComposeOperator 364
#define OptionErrorUnrecognizedDisposeMethod 365
#define OptionErrorUnrecognizedElement 366
#define OptionErrorUnrecognizedEndianType 367
#define OptionErrorUnrecognizedGravityType 368
#define OptionErrorUnrecognizedImageCompression 369
#define OptionErrorUnrecognizedImageFilter 370
#define OptionErrorUnrecognizedImageFormat 371
#define OptionErrorUnrecognizedImageMode 372
#define OptionErrorUnrecognizedImageType 373
#define OptionErrorUnrecognizedIntentType 374
#define OptionErrorUnrecognizedInterlaceType 375
#define OptionErrorUnrecognizedListType 376
#define OptionErrorUnrecognizedModeType 377
#define OptionErrorUnrecognizedNoiseType 378
#define OptionErrorUnrecognizedOption 379
#define OptionErrorUnrecognizedPerlMagickMethod 380
#define OptionErrorUnrecognizedPixelMap 381
#define OptionErrorUnrecognizedPreviewType 382
#define OptionErrorUnrecognizedResourceType 383
#define OptionErrorUnrecognizedType 384
#define OptionErrorUnrecognizedVirtualPixelMethod 385
#define OptionErrorUsageError 386
#define OptionFatalErrorDeprecatedOption 387
#define OptionFatalErrorInvalidColorspaceType 388
#define OptionFatalErrorInvalidEndianType 389
#define OptionFatalErrorInvalidImageType 390
#define OptionFatalErrorInvalidInterlaceType 391
#define OptionFatalErrorMissingAnImageFilename 392
#define OptionFatalErrorMissingBevelWidth 393
#define OptionFatalErrorMissingBorderColor 394
#define OptionFatalErrorMissingColor 395
#define OptionFatalErrorMissingColors 396
#define OptionFatalErrorMissingCommand 397
#define OptionFatalErrorMissingComment 398
#define OptionFatalErrorMissingCompressionQuality 399
#define OptionFatalErrorMissingDegrees 400
#define OptionFatalErrorMissingDepth 401
#define OptionFatalErrorMissingDisposeMethod 402
#define OptionFatalErrorMissingEncodingType 403
#define OptionFatalErrorMissingEventMask 404
#define OptionFatalErrorMissingFactor 405
#define OptionFatalErrorMissingFilename 406
#define OptionFatalErrorMissingFontName 407
#define OptionFatalErrorMissingForeground 408
#define OptionFatalErrorMissingGeometry 409
#define OptionFatalErrorMissingIDNameOrRoot 410
#define OptionFatalErrorMissingImageDepth 411
#define OptionFatalErrorMissingLabelName 412
#define OptionFatalErrorMissingLevel 413
#define OptionFatalErrorMissingLogFormat 414
#define OptionFatalErrorMissingMapType 415
#define OptionFatalErrorMissingMethod 416
#define OptionFatalErrorMissingName 417
#define OptionFatalErrorMissingPageGeometry 418
#define OptionFatalErrorMissingPassword 419
#define OptionFatalErrorMissingQuality 420
#define OptionFatalErrorMissingResourceLimit 421
#define OptionFatalErrorMissingResourceType 422
#define OptionFatalErrorMissingSceneNumber 423
#define OptionFatalErrorMissingSeconds 424
#define OptionFatalErrorMissingServerName 425
#define OptionFatalErrorMissingSize 426
#define OptionFatalErrorMissingSnaps 427
#define OptionFatalErrorMissingThreshold 428
#define OptionFatalErrorMissingTitle 429
#define OptionFatalErrorMissingType 430
#define OptionFatalErrorMissingValue 431
#define OptionFatalErrorMissingVirtualPixelMethod 432
#define OptionFatalErrorMissingVisualClass 433
#define OptionFatalErrorMissingWidth 434
#define OptionFatalErrorNoImagesWereLoaded 435
#define OptionFatalErrorOptionLengthExceedsLimit 436
#define OptionFatalErrorUnableToOpenXServer 437
#define OptionFatalErrorUnableToPersistKey 438
#define OptionFatalErrorUnrecognizedColormapType 439
#define OptionFatalErrorUnrecognizedColorspaceType 440
#define OptionFatalErrorUnrecognizedDisposeMethod 441
#define OptionFatalErrorUnrecognizedEndianType 442
#define OptionFatalErrorUnrecognizedFilterType 443
#define OptionFatalErrorUnrecognizedImageCompressionType 444
#define OptionFatalErrorUnrecognizedImageType 445
#define OptionFatalErrorUnrecognizedInterlaceType 446
#define OptionFatalErrorUnrecognizedOption 447
#define OptionFatalErrorUnrecognizedResourceType 448
#define OptionFatalErrorUnrecognizedVirtualPixelMethod 449
#define OptionWarningUnrecognizedColor 450
#define RegistryErrorImageExpected 451
#define RegistryErrorImageInfoExpected 452
#define RegistryErrorStructureSizeMismatch 453
#define RegistryErrorUnableToGetRegistryID 454
#define RegistryErrorUnableToLocateImage 455
#define RegistryErrorUnableToSetRegistry 456
#define RegistryFatalErrorDefault 457
#define RegistryWarningDefault 458
#define ResourceLimitErrorCacheResourcesExhausted 459
#define ResourceLimitErrorMemoryAllocationFailed 460
#define ResourceLimitErrorNoPixelsDefinedInCache 461
#define ResourceLimitErrorPixelCacheAllocationFailed 462
#define ResourceLimitErrorUnableToAddColorProfile 463
#define ResourceLimitErrorUnableToAddGenericProfile 464
#define ResourceLimitErrorUnableToAddIPTCProfile 465
#define ResourceLimitErrorUnableToAllocateCoefficients 466
#define ResourceLimitErrorUnableToAllocateColormap 467
#define ResourceLimitErrorUnableToAllocateString 468
#define ResourceLimitErrorUnableToAnnotateImage 469
#define ResourceLimitErrorUnableToAverageImageSequence 470
#define ResourceLimitErrorUnableToCloneImage 471
#define ResourceLimitErrorUnableToComputeImageSignature 472
#define ResourceLimitErrorUnableToConstituteImage 473
#define ResourceLimitErrorUnableToConvertFont 474
#define ResourceLimitErrorUnableToConvertStringToTokens 475
#define ResourceLimitErrorUnableToCreateColormap 476
#define ResourceLimitErrorUnableToCreateColorTransform 477
#define ResourceLimitErrorUnableToCreateCommandWidget 478
#define ResourceLimitErrorUnableToCreateImageGroup 479
#define ResourceLimitErrorUnableToCreateImageMontage 480
#define ResourceLimitErrorUnableToCreateXWindow 481
#define ResourceLimitErrorUnableToCropImage 482
#define ResourceLimitErrorUnableToDespeckleImage 483
#define ResourceLimitErrorUnableToDetermineImageClass 484
#define ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 485
#define ResourceLimitErrorUnableToDitherImage 486
#define ResourceLimitErrorUnableToDrawOnImage 487
#define ResourceLimitErrorUnableToEdgeImage 488
#define ResourceLimitErrorUnableToEmbossImage 489
#define ResourceLimitErrorUnableToEnhanceImage 490
#define ResourceLimitErrorUnableToFloodfillImage 491
#define ResourceLimitErrorUnableToGammaCorrectImage 492
#define ResourceLimitErrorUnableToGetBestIconSize 493
#define ResourceLimitErrorUnableToGetFromRegistry 494
#define ResourceLimitErrorUnableToGetPackageInfo 495
#define ResourceLimitErrorUnableToLevelImage 496
#define ResourceLimitErrorUnableToMagnifyImage 497
#define ResourceLimitErrorUnableToManageColor 498
#define ResourceLimitErrorUnableToMapImage 499
#define ResourceLimitErrorUnableToMapImageSequence 500
#define ResourceLimitErrorUnableToMedianFilterImage 501
#define ResourceLimitErrorUnableToMotionBlurImage 502
#define ResourceLimitErrorUnableToNoiseFilterImage 503
#define ResourceLimitErrorUnableToNormalizeImage 504
#define ResourceLimitErrorUnableToOpenColorProfile 505
#define ResourceLimitErrorUnableToQuantizeImage 506
#define ResourceLimitErrorUnableToQuantizeImageSequence 507
#define ResourceLimitErrorUnableToReadTextChunk 508
#define ResourceLimitErrorUnableToReadXImage 509
#define ResourceLimitErrorUnableToReadXServerColormap 510
#define ResourceLimitErrorUnableToResizeImage 511
#define ResourceLimitErrorUnableToRotateImage 512
#define ResourceLimitErrorUnableToSampleImage 513
#define ResourceLimitErrorUnableToScaleImage 514
#define ResourceLimitErrorUnableToSelectImage 515
#define ResourceLimitErrorUnableToSharpenImage 516
#define ResourceLimitErrorUnableToShaveImage 517
#define ResourceLimitErrorUnableToShearImage 518
#define ResourceLimitErrorUnableToSortImageColormap 519
#define ResourceLimitErrorUnableToThresholdImage 520
#define ResourceLimitErrorUnableToTransformColorspace 521
#define ResourceLimitFatalErrorMemoryAllocationFailed 522
#define ResourceLimitFatalErrorUnableToAllocateAscii85Info 523
#define ResourceLimitFatalErrorUnableToAllocateCacheInfo 524
#define ResourceLimitFatalErrorUnableToAllocateCacheView 525
#define ResourceLimitFatalErrorUnableToAllocateColorInfo 526
#define ResourceLimitFatalErrorUnableToAllocateDashPattern 527
#define ResourceLimitFatalErrorUnableToAllocateDelegateInfo 528
#define ResourceLimitFatalErrorUnableToAllocateDerivatives 529
#define ResourceLimitFatalErrorUnableToAllocateDrawContext 530
#define ResourceLimitFatalErrorUnableToAllocateDrawInfo 531
#define ResourceLimitFatalErrorUnableToAllocateGammaMap 532
#define ResourceLimitFatalErrorUnableToAllocateImage 533
#define ResourceLimitFatalErrorUnableToAllocateImagePixels 534
#define ResourceLimitFatalErrorUnableToAllocateLogInfo 535
#define ResourceLimitFatalErrorUnableToAllocateMagicInfo 536
#define ResourceLimitFatalErrorUnableToAllocateMagickInfo 537
#define ResourceLimitFatalErrorUnableToAllocateModuleInfo 538
#define ResourceLimitFatalErrorUnableToAllocateMontageInfo 539
#define ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 540
#define ResourceLimitFatalErrorUnableToAllocateRegistryInfo 541
#define ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 542
#define ResourceLimitFatalErrorUnableToAllocateString 543
#define ResourceLimitFatalErrorUnableToAllocateTypeInfo 544
#define ResourceLimitFatalErrorUnableToAnimateImageSequence 545
#define ResourceLimitFatalErrorUnableToCloneBlobInfo 546
#define ResourceLimitFatalErrorUnableToCloneCacheInfo 547
#define ResourceLimitFatalErrorUnableToCloneImage 548
#define ResourceLimitFatalErrorUnableToCloneImageInfo 549
#define ResourceLimitFatalErrorUnableToConcatenateString 550
#define ResourceLimitFatalErrorUnableToConvertText 551
#define ResourceLimitFatalErrorUnableToCreateColormap 552
#define ResourceLimitFatalErrorUnableToDisplayImage 553
#define ResourceLimitFatalErrorUnableToEscapeString 554
#define ResourceLimitFatalErrorUnableToInterpretMSLImage 555
#define ResourceLimitWarningMemoryAllocationFailed 556
#define StreamErrorImageDoesNotContainTheStreamGeometry 557
#define StreamErrorNoStreamHandlerIsDefined 558
#define StreamErrorPixelCacheIsNotOpen 559
#define StreamErrorUnableToAcquirePixelStream 560
#define StreamErrorUnableToSetPixelStream 561
#define StreamErrorUnableToSyncPixelStream 562
#define StreamFatalErrorDefault 563
#define StreamWarningDefault 564
#define TypeErrorFontSubstitutionRequired 565
#define TypeErrorUnableToGetTypeMetrics 566
#define TypeErrorUnableToInitializeFreetypeLibrary 567
#define TypeErrorUnableToReadFont 568
#define TypeErrorUnrecognizedFontEncoding 569
#define TypeFatalErrorDefault 570
#define TypeWarningDefault 571
#define XServerErrorColorIsNotKnownToServer 572
#define XServerErrorNoWindowWithSpecifiedIDExists 573
#define XServerErrorStandardColormapIsNotInitialized 574
#define XServerErrorUnableToConnectToRemoteDisplay 575
#define XServerErrorUnableToCreateBitmap 576
#define XServerErrorUnableToCreateColormap 577
#define XServerErrorUnableToCreatePixmap 578
#define XServerErrorUnableToCreateProperty 579
#define XServerErrorUnableToCreateStandardColormap 580
#define XServerErrorUnableToDisplayImageInfo 581
#define XServerErrorUnableToGetProperty 582
#define XServerErrorUnableToGetStandardColormap 583
#define XServerErrorUnableToGetVisual 584
#define XServerErrorUnableToGrabMouse 585
#define XServerErrorUnableToLoadFont 586
#define XServerErrorUnableToMatchVisualToStandardColormap 587
#define XServerErrorUnableToOpenXServer 588
#define XServerErrorUnableToReadXAttributes 589
#define XServerErrorUnableToReadXWindowImage 590
#define XServerErrorUnrecognizedColormapType 591
#define XServerErrorUnrecognizedGravityType 592
#define XServerErrorUnrecognizedVisualSpecifier 593
#define XServerFatalErrorUnableToAllocateXHints 594
#define XServerFatalErrorUnableToCreateCursor 595
#define XServerFatalErrorUnableToCreateGraphicContext 596
#define XServerFatalErrorUnableToCreateStandardColormap 597
#define XServerFatalErrorUnableToCreateTextProperty 598
#define XServerFatalErrorUnableToCreateXImage 599
#define XServerFatalErrorUnableToCreateXPixmap 600
#define XServerFatalErrorUnableToCreateXWindow 601
#define XServerFatalErrorUnableToDisplayImage 602
#define XServerFatalErrorUnableToDitherImage 603
#define XServerFatalErrorUnableToGetPixelInfo 604
#define XServerFatalErrorUnableToGetVisual 605
#define XServerFatalErrorUnableToLoadFont 606
#define XServerFatalErrorUnableToMakeXWindow 607
#define XServerFatalErrorUnableToOpenXServer 608
#define XServerFatalErrorUnableToViewFonts 609
#define XServerWarningUnableToGetVisual 610
#define XServerWarningUsingDefaultVisual 611

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
    { "Coder/FatalError", 82, CoderFatalError },
    { "Coder/Warning", 83, CoderWarning },
    { "Configure/Error", 84, ConfigureError },
    { "Configure/FatalError", 90, ConfigureFatalError },
    { "Configure/Warning", 91, ConfigureWarning },
    { "Corrupt/Image/Error", 92, CorruptImageError },
    { "Corrupt/Image/FatalError", 168, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 169, CorruptImageWarning },
    { "Delegate/Error", 178, DelegateError },
    { "Delegate/FatalError", 192, DelegateFatalError },
    { "Delegate/Warning", 193, DelegateWarning },
    { "Draw/Error", 194, DrawError },
    { "Draw/FatalError", 198, DrawFatalError },
    { "Draw/Warning", 199, DrawWarning },
    { "File/Open/Error", 202, FileOpenError },
    { "File/Open/FatalError", 205, FileOpenFatalError },
    { "File/Open/Warning", 206, FileOpenWarning },
    { "Image/Error", 207, ImageError },
    { "Image/FatalError", 230, ImageFatalError },
    { "Image/Warning", 231, ImageWarning },
    { "Missing/Delegate/Error", 232, MissingDelegateError },
    { "Missing/Delegate/FatalError", 243, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 244, MissingDelegateWarning },
    { "Module/Error", 245, ModuleError },
    { "Module/FatalError", 250, ModuleFatalError },
    { "Module/Warning", 251, ModuleWarning },
    { "Option/Error", 252, OptionError },
    { "Option/FatalError", 386, OptionFatalError },
    { "Option/Warning", 449, OptionWarning },
    { "Registry/Error", 450, RegistryError },
    { "Registry/FatalError", 456, RegistryFatalError },
    { "Registry/Warning", 457, RegistryWarning },
    { "Resource/Limit/Error", 458, ResourceLimitError },
    { "Resource/Limit/FatalError", 521, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 555, ResourceLimitWarning },
    { "Stream/Error", 556, StreamError },
    { "Stream/FatalError", 562, StreamFatalError },
    { "Stream/Warning", 563, StreamWarning },
    { "Type/Error", 564, TypeError },
    { "Type/FatalError", 569, TypeFatalError },
    { "Type/Warning", 570, TypeWarning },
    { "XServer/Error", 571, XServerError },
    { "XServer/FatalError", 593, XServerFatalError },
    { "XServer/Warning", 609, XServerWarning },
    { 0, 611, 0 }
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
    { "JPEGCompressionNotSupported", 38 },
    { "JPEGEmbeddingFailed", 39 },
    { "LocationTypeIsNotSupported", 40 },
    { "MapStorageTypeIsNotSupported", 41 },
    { "MSBByteOrderNotSupported", 42 },
    { "MultidimensionalMatricesAreNotSupported", 43 },
    { "MultipleRecordListNotSupported", 44 },
    { "No8BIMDataIsAvailable", 45 },
    { "NoAPP1DataIsAvailable", 46 },
    { "NoBitmapOnClipboard", 47 },
    { "NoColorProfileAvailable", 48 },
    { "NoDataReturned", 49 },
    { "NoImageVectorGraphics", 50 },
    { "NoIPTCInfoWasFound", 51 },
    { "NoIPTCProfileAvailable", 52 },
    { "NotASupportedImageFile", 53 },
    { "NumberOfImagesIsNotSupported", 54 },
    { "OnlyContinuousTonePictureSupported", 55 },
    { "OnlyLevelZerofilesSupported", 56 },
    { "PNGCompressionNotSupported", 57 },
    { "RLECompressionNotSupported", 58 },
    { "UnableToCopyProfile", 59 },
    { "UnableToCreateADC", 60 },
    { "UnableToCreateBitmap", 61 },
    { "UnableToDecompressImage", 62 },
    { "UnableToInitializeFPXLibrary", 63 },
    { "UnableToOpenBlob", 64 },
    { "UnableToReadAspectRatio", 65 },
    { "UnableToReadCIELABImages", 66 },
    { "UnableToReadSummaryInfo", 67 },
    { "UnableToSetAffineMatrix", 68 },
    { "UnableToSetAspectRatio", 69 },
    { "UnableToSetColorTwist", 70 },
    { "UnableToSetContrast", 71 },
    { "UnableToSetFilteringValue", 72 },
    { "UnableToSetImageComments", 73 },
    { "UnableToSetImageTitle", 74 },
    { "UnableToSetJPEGLevel", 75 },
    { "UnableToSetRegionOfInterest", 76 },
    { "UnableToSetSummaryInfo", 77 },
    { "UnableToTranslateText", 78 },
    { "UnableToWriteMPEGParameters", 79 },
    { "UnableToZipCompressImage", 80 },
    { "UnsupportedCellTypeInTheMatrix", 81 },
    { "ZipCompressionNotSupported", 82 },
    { "Default", 83 },
    { "LosslessToLossyJPEGConversion", 84 },
    { "IncludeElementNestedTooDeeply", 85 },
    { "RegistryKeyLookupFailed", 86 },
    { "UnableToAccessConfigureFile", 87 },
    { "UnableToAccessFontFile", 88 },
    { "UnableToAccessLogFile", 89 },
    { "UnableToAccessModuleFile", 90 },
    { "Default", 91 },
    { "Default", 92 },
    { "AnErrorHasOccurredReadingFromFile", 93 },
    { "AnErrorHasOccurredWritingToFile", 94 },
    { "ColormapExceeds256Colors", 95 },
    { "CorruptImage", 96 },
    { "ImageFileDoesNotContainAnyImageData", 97 },
    { "ImageTypeNotSupported", 98 },
    { "ImproperImageHeader", 99 },
    { "InsufficientImageDataInFile", 100 },
    { "InvalidColormapIndex", 101 },
    { "InvalidFileFormatVersion", 102 },
    { "LengthAndFilesizeDoNotMatch", 103 },
    { "MissingImageChannel", 104 },
    { "NegativeOrZeroImageSize", 105 },
    { "NonOS2HeaderSizeError", 106 },
    { "NotABMPImageFile", 107 },
    { "NotACUTImageFile", 108 },
    { "NotADCMImageFile", 109 },
    { "NotADCXImageFile", 110 },
    { "NotADIBImageFile", 111 },
    { "NotADPXImageFile", 112 },
    { "NotAGIFImageFile", 113 },
    { "NotAJNGImageFile", 114 },
    { "NotAMATLABImageFile", 115 },
    { "NotAMNGImageFile", 116 },
    { "NotAMTVImageFile", 117 },
    { "NotAnARTImageFile", 118 },
    { "NotAnAVIImageFile", 119 },
    { "NotAnAVSImageFile", 120 },
    { "NotAnEMFImageFile", 121 },
    { "NotAnICOImageFile", 122 },
    { "NotAOTBImageFile", 123 },
    { "NotAPCDImageFile", 124 },
    { "NotAPCXImageFile", 125 },
    { "NotAPDBImageFile", 126 },
    { "NotAPICTImageFile", 127 },
    { "NotAPIXImageFile", 128 },
    { "NotAPNGImageFile", 129 },
    { "NotAPNMImageFile", 130 },
    { "NotAPSDImageFile", 131 },
    { "NotAPWPImageFile", 132 },
    { "NotARLEImageFile", 133 },
    { "NotASCTImageFile", 134 },
    { "NotASFWImageFile", 135 },
    { "NotASGIImageFile", 136 },
    { "NotASUNImageFile", 137 },
    { "NotATGAImageFile", 138 },
    { "NotATIMImageFile", 139 },
    { "NotATOPOLImageFile", 140 },
    { "NotAVIFFImageFile", 141 },
    { "NotAWBMPImageFile", 142 },
    { "NotAWPGImageFile", 143 },
    { "NotAXBMImageFile", 144 },
    { "NotAXCFImageFile", 145 },
    { "NotAXPMImageFile", 146 },
    { "NotEnoughTiles", 147 },
    { "StaticPlanesValueNotEqualToOne", 148 },
    { "TooMuchImageDataInFile", 149 },
    { "UnableToReadColormapFromDumpFile", 150 },
    { "UnableToReadColorProfile", 151 },
    { "UnableToReadExtensionBlock", 152 },
    { "UnableToReadGenericProfile", 153 },
    { "UnableToReadImageData", 154 },
    { "UnableToReadImageHeader", 155 },
    { "UnableToReadIPTCProfile", 156 },
    { "UnableToReadPixmapFromDumpFile", 157 },
    { "UnableToReadSubImageData", 158 },
    { "UnableToReadVIDImage", 159 },
    { "UnableToReadWindowNameFromDumpFile", 160 },
    { "UnableToRunlengthDecodeImage", 161 },
    { "UnableToUncompressImage", 162 },
    { "UnexpectedEndOfFile", 163 },
    { "UnexpectedSamplingFactor", 164 },
    { "UnknownPatternType", 165 },
    { "UnrecognizedBitsPerPixel", 166 },
    { "UnrecognizedImageCompression", 167 },
    { "UnrecognizedXWDHeader", 168 },
    { "UnableToPersistKey", 169 },
    { "CompressionNotValid", 170 },
    { "ImproperImageHeader", 171 },
    { "NegativeOrZeroImageSize", 172 },
    { "NonOS2HeaderSizeError", 173 },
    { "SkipToSyncByte", 174 },
    { "StaticPlanesValueNotEqualToOne", 175 },
    { "UnrecognizedBitsPerPixel", 176 },
    { "UnrecognizedImageCompression", 177 },
    { "UnrecognizedNumberOfColors", 178 },
    { "DelegateFailed", 179 },
    { "FailedToComputeOutputSize", 180 },
    { "FailedToRenderFile", 181 },
    { "FailedToScanFile", 182 },
    { "NoTagFound", 183 },
    { "PostscriptDelegateFailed", 184 },
    { "UnableToCreateImage", 185 },
    { "UnableToCreateImageComponent", 186 },
    { "UnableToDecodeImageFile", 187 },
    { "UnableToEncodeImageFile", 188 },
    { "UnableToInitializeFPXLibrary", 189 },
    { "UnableToInitializeWMFLibrary", 190 },
    { "UnableToManageJP2Stream", 191 },
    { "UnableToWriteSVGFormat", 192 },
    { "Default", 193 },
    { "Default", 194 },
    { "AlreadyPushingPatternDefinition", 195 },
    { "NonconformingDrawingPrimitiveDefinition", 196 },
    { "UnableToPrint", 197 },
    { "UnbalancedGraphicContextPushPop", 198 },
    { "Default", 199 },
    { "NotARelativeURL", 200 },
    { "NotCurrentlyPushingPatternDefinition", 201 },
    { "URLNotFound", 202 },
    { "UnableToCreateTemporaryFile", 203 },
    { "UnableToOpenFile", 204 },
    { "UnableToWriteFile", 205 },
    { "Default", 206 },
    { "Default", 207 },
    { "AngleIsDiscontinuous", 208 },
    { "ImageColorspaceDiffers", 209 },
    { "ImageOpacityDiffers", 210 },
    { "ImageSequenceIsRequired", 211 },
    { "ImageSizeDiffers", 212 },
    { "LeftAndRightImageSizesDiffer", 213 },
    { "NoImagesWereFound", 214 },
    { "NoImagesWereLoaded", 215 },
    { "NoLocaleImageAttribute", 216 },
    { "TooManyClusters", 217 },
    { "UnableToAppendImage", 218 },
    { "UnableToAverageImage", 219 },
    { "UnableToCoalesceImage", 220 },
    { "UnableToCompareImages", 221 },
    { "UnableToCreateImageMosaic", 222 },
    { "UnableToCreateStereoImage", 223 },
    { "UnableToDeconstructImageSequence", 224 },
    { "UnableToFlattenImage", 225 },
    { "UnableToResizeImage", 226 },
    { "UnableToSegmentImage", 227 },
    { "UnableToSetClipMask", 228 },
    { "UnableToShearImage", 229 },
    { "WidthOrHeightExceedsLimit", 230 },
    { "UnableToPersistKey", 231 },
    { "Default", 232 },
    { "DPSLibraryIsNotAvailable", 233 },
    { "FPXLibraryIsNotAvailable", 234 },
    { "FreeTypeLibraryIsNotAvailable", 235 },
    { "JPEGLibraryIsNotAvailable", 236 },
    { "LZWEncodingNotEnabled", 237 },
    { "NoDecodeDelegateForThisImageFormat", 238 },
    { "NoEncodeDelegateForThisImageFormat", 239 },
    { "TIFFLibraryIsNotAvailable", 240 },
    { "XMLLibraryIsNotAvailable", 241 },
    { "XWindowLibraryIsNotAvailable", 242 },
    { "ZipLibraryIsNotAvailable", 243 },
    { "Default", 244 },
    { "Default", 245 },
    { "FailedToCloseModule", 246 },
    { "FailedToFindSymbol", 247 },
    { "UnableToLoadModule", 248 },
    { "UnableToRegisterImageFormat", 249 },
    { "UnrecognizedModule", 250 },
    { "UnableToInitializeModuleLoader", 251 },
    { "Default", 252 },
    { "BevelWidthIsNegative", 253 },
    { "ColorSeparatedImageRequired", 254 },
    { "FrameIsLessThanImageSize", 255 },
    { "GeometryDimensionsAreZero", 256 },
    { "GeometryDoesNotContainImage", 257 },
    { "ImagesAreNotTheSameSize", 258 },
    { "ImageSizeMustExceedBevelWidth", 259 },
    { "ImageSmallerThanKernelWidth", 260 },
    { "ImageSmallerThanRadius", 261 },
    { "ImageWidthsOrHeightsDiffer", 262 },
    { "InputImagesAlreadySpecified", 263 },
    { "KernelRadiusIsTooSmall", 264 },
    { "KernelWidthMustBeAnOddNumber", 265 },
    { "MissingAffineMatrix", 266 },
    { "MissingAmount", 267 },
    { "MissingAmplitude", 268 },
    { "MissingAnImageFilename", 269 },
    { "MissingArgument", 270 },
    { "MissingAzimuth", 271 },
    { "MissingBackgroundColor", 272 },
    { "MissingBevelWidth", 273 },
    { "MissingBorderColor", 274 },
    { "MissingCoefficients", 275 },
    { "MissingColorizeValue", 276 },
    { "MissingColors", 277 },
    { "MissingComment", 278 },
    { "MissingCompressionQuality", 279 },
    { "MissingCycleAmount", 280 },
    { "MissingDelay", 281 },
    { "MissingDisposeMethod", 282 },
    { "MissingDissolveValue", 283 },
    { "MissingDistance", 284 },
    { "MissingDrawingPrimitive", 285 },
    { "MissingEnodingType", 286 },
    { "MissingEventMask", 287 },
    { "MissingFilename", 288 },
    { "MissingFillColor", 289 },
    { "MissingFontName", 290 },
    { "MissingFormatSpecifier", 291 },
    { "MissingGeometry", 292 },
    { "MissingImageDepth", 293 },
    { "MissingImageFrames", 294 },
    { "MissingImageGamma", 295 },
    { "MissingImageIterations", 296 },
    { "MissingImageLabel", 297 },
    { "MissingImageProfile", 298 },
    { "MissingImageRotation", 299 },
    { "MissingImageScene", 300 },
    { "MissingImageType", 301 },
    { "MissingImplodeAmount", 302 },
    { "MissingListName", 303 },
    { "MissingLogFormat", 304 },
    { "MissingMatrix", 305 },
    { "MissingMatteColor", 306 },
    { "MissingOffset", 307 },
    { "MissingOpaqueColor", 308 },
    { "MissingOrder", 309 },
    { "MissingPageGeometry", 310 },
    { "MissingPaintRadius", 311 },
    { "MissingPassword", 312 },
    { "MissingPoint", 313 },
    { "MissingPointsize", 314 },
    { "MissingResourceLimit", 315 },
    { "MissingResourceType", 316 },
    { "MissingSeedValue", 317 },
    { "MissingServerName", 318 },
    { "MissingSpreadAmount", 319 },
    { "MissingThreshold", 320 },
    { "MissingTile", 321 },
    { "MissingTitle", 322 },
    { "MissingTransparentColor", 323 },
    { "MissingTreeDepth", 324 },
    { "MissingType", 325 },
    { "MissingViewTransform", 326 },
    { "MissingVirtualPixelMethod", 327 },
    { "MissingWidth", 328 },
    { "MustSpecifyAnImageName", 329 },
    { "MustSpecifyImageSize", 330 },
    { "NoBlobDefined", 331 },
    { "NoImagesDefined", 332 },
    { "NonzeroWidthAndHeightRequired", 333 },
    { "NoProfileNameWasGiven", 334 },
    { "NullBlobArgument", 335 },
    { "ReferenceImageRequired", 336 },
    { "ReferenceIsNotMyType", 337 },
    { "SteganoImageRequired", 338 },
    { "StereoImageRequired", 339 },
    { "SubimageSpecificationReturnsNoImages", 340 },
    { "UnableToAddOrRemoveProfile", 341 },
    { "UnableToAllocateICCProfile", 342 },
    { "UnableToAverageImageSequence", 343 },
    { "UnableToBlurImage", 344 },
    { "UnableToChopImage", 345 },
    { "UnableToConstituteImage", 346 },
    { "UnableToConvolveImage", 347 },
    { "UnableToEdgeImage", 348 },
    { "UnableToEqualizeImage", 349 },
    { "UnableToFilterImage", 350 },
    { "UnableToFormatImageMetadata", 351 },
    { "UnableToFrameImage", 352 },
    { "UnableToOilPaintImage", 353 },
    { "UnableToPaintImage", 354 },
    { "UnableToRaiseImage", 355 },
    { "UnableToSharpenImage", 356 },
    { "UnableToThresholdImage", 357 },
    { "UnableToWaveImage", 358 },
    { "UnrecognizedAttribute", 359 },
    { "UnrecognizedChannelType", 360 },
    { "UnrecognizedColor", 361 },
    { "UnrecognizedColormapType", 362 },
    { "UnrecognizedColorspace", 363 },
    { "UnrecognizedComposeOperator", 364 },
    { "UnrecognizedDisposeMethod", 365 },
    { "UnrecognizedElement", 366 },
    { "UnrecognizedEndianType", 367 },
    { "UnrecognizedGravityType", 368 },
    { "UnrecognizedImageCompression", 369 },
    { "UnrecognizedImageFilter", 370 },
    { "UnrecognizedImageFormat", 371 },
    { "UnrecognizedImageMode", 372 },
    { "UnrecognizedImageType", 373 },
    { "UnrecognizedIntentType", 374 },
    { "UnrecognizedInterlaceType", 375 },
    { "UnrecognizedListType", 376 },
    { "UnrecognizedModeType", 377 },
    { "UnrecognizedNoiseType", 378 },
    { "UnrecognizedOption", 379 },
    { "UnrecognizedPerlMagickMethod", 380 },
    { "UnrecognizedPixelMap", 381 },
    { "UnrecognizedPreviewType", 382 },
    { "UnrecognizedResourceType", 383 },
    { "UnrecognizedType", 384 },
    { "UnrecognizedVirtualPixelMethod", 385 },
    { "UsageError", 386 },
    { "DeprecatedOption", 387 },
    { "InvalidColorspaceType", 388 },
    { "InvalidEndianType", 389 },
    { "InvalidImageType", 390 },
    { "InvalidInterlaceType", 391 },
    { "MissingAnImageFilename", 392 },
    { "MissingBevelWidth", 393 },
    { "MissingBorderColor", 394 },
    { "MissingColor", 395 },
    { "MissingColors", 396 },
    { "MissingCommand", 397 },
    { "MissingComment", 398 },
    { "MissingCompressionQuality", 399 },
    { "MissingDegrees", 400 },
    { "MissingDepth", 401 },
    { "MissingDisposeMethod", 402 },
    { "MissingEncodingType", 403 },
    { "MissingEventMask", 404 },
    { "MissingFactor", 405 },
    { "MissingFilename", 406 },
    { "MissingFontName", 407 },
    { "MissingForeground", 408 },
    { "MissingGeometry", 409 },
    { "MissingIDNameOrRoot", 410 },
    { "MissingImageDepth", 411 },
    { "MissingLabelName", 412 },
    { "MissingLevel", 413 },
    { "MissingLogFormat", 414 },
    { "MissingMapType", 415 },
    { "MissingMethod", 416 },
    { "MissingName", 417 },
    { "MissingPageGeometry", 418 },
    { "MissingPassword", 419 },
    { "MissingQuality", 420 },
    { "MissingResourceLimit", 421 },
    { "MissingResourceType", 422 },
    { "MissingSceneNumber", 423 },
    { "MissingSeconds", 424 },
    { "MissingServerName", 425 },
    { "MissingSize", 426 },
    { "MissingSnaps", 427 },
    { "MissingThreshold", 428 },
    { "MissingTitle", 429 },
    { "MissingType", 430 },
    { "MissingValue", 431 },
    { "MissingVirtualPixelMethod", 432 },
    { "MissingVisualClass", 433 },
    { "MissingWidth", 434 },
    { "NoImagesWereLoaded", 435 },
    { "OptionLengthExceedsLimit", 436 },
    { "UnableToOpenXServer", 437 },
    { "UnableToPersistKey", 438 },
    { "UnrecognizedColormapType", 439 },
    { "UnrecognizedColorspaceType", 440 },
    { "UnrecognizedDisposeMethod", 441 },
    { "UnrecognizedEndianType", 442 },
    { "UnrecognizedFilterType", 443 },
    { "UnrecognizedImageCompressionType", 444 },
    { "UnrecognizedImageType", 445 },
    { "UnrecognizedInterlaceType", 446 },
    { "UnrecognizedOption", 447 },
    { "UnrecognizedResourceType", 448 },
    { "UnrecognizedVirtualPixelMethod", 449 },
    { "UnrecognizedColor", 450 },
    { "ImageExpected", 451 },
    { "ImageInfoExpected", 452 },
    { "StructureSizeMismatch", 453 },
    { "UnableToGetRegistryID", 454 },
    { "UnableToLocateImage", 455 },
    { "UnableToSetRegistry", 456 },
    { "Default", 457 },
    { "Default", 458 },
    { "CacheResourcesExhausted", 459 },
    { "MemoryAllocationFailed", 460 },
    { "NoPixelsDefinedInCache", 461 },
    { "PixelCacheAllocationFailed", 462 },
    { "UnableToAddColorProfile", 463 },
    { "UnableToAddGenericProfile", 464 },
    { "UnableToAddIPTCProfile", 465 },
    { "UnableToAllocateCoefficients", 466 },
    { "UnableToAllocateColormap", 467 },
    { "UnableToAllocateString", 468 },
    { "UnableToAnnotateImage", 469 },
    { "UnableToAverageImageSequence", 470 },
    { "UnableToCloneImage", 471 },
    { "UnableToComputeImageSignature", 472 },
    { "UnableToConstituteImage", 473 },
    { "UnableToConvertFont", 474 },
    { "UnableToConvertStringToTokens", 475 },
    { "UnableToCreateColormap", 476 },
    { "UnableToCreateColorTransform", 477 },
    { "UnableToCreateCommandWidget", 478 },
    { "UnableToCreateImageGroup", 479 },
    { "UnableToCreateImageMontage", 480 },
    { "UnableToCreateXWindow", 481 },
    { "UnableToCropImage", 482 },
    { "UnableToDespeckleImage", 483 },
    { "UnableToDetermineImageClass", 484 },
    { "UnableToDetermineTheNumberOfImageColors", 485 },
    { "UnableToDitherImage", 486 },
    { "UnableToDrawOnImage", 487 },
    { "UnableToEdgeImage", 488 },
    { "UnableToEmbossImage", 489 },
    { "UnableToEnhanceImage", 490 },
    { "UnableToFloodfillImage", 491 },
    { "UnableToGammaCorrectImage", 492 },
    { "UnableToGetBestIconSize", 493 },
    { "UnableToGetFromRegistry", 494 },
    { "UnableToGetPackageInfo", 495 },
    { "UnableToLevelImage", 496 },
    { "UnableToMagnifyImage", 497 },
    { "UnableToManageColor", 498 },
    { "UnableToMapImage", 499 },
    { "UnableToMapImageSequence", 500 },
    { "UnableToMedianFilterImage", 501 },
    { "UnableToMotionBlurImage", 502 },
    { "UnableToNoiseFilterImage", 503 },
    { "UnableToNormalizeImage", 504 },
    { "UnableToOpenColorProfile", 505 },
    { "UnableToQuantizeImage", 506 },
    { "UnableToQuantizeImageSequence", 507 },
    { "UnableToReadTextChunk", 508 },
    { "UnableToReadXImage", 509 },
    { "UnableToReadXServerColormap", 510 },
    { "UnableToResizeImage", 511 },
    { "UnableToRotateImage", 512 },
    { "UnableToSampleImage", 513 },
    { "UnableToScaleImage", 514 },
    { "UnableToSelectImage", 515 },
    { "UnableToSharpenImage", 516 },
    { "UnableToShaveImage", 517 },
    { "UnableToShearImage", 518 },
    { "UnableToSortImageColormap", 519 },
    { "UnableToThresholdImage", 520 },
    { "UnableToTransformColorspace", 521 },
    { "MemoryAllocationFailed", 522 },
    { "UnableToAllocateAscii85Info", 523 },
    { "UnableToAllocateCacheInfo", 524 },
    { "UnableToAllocateCacheView", 525 },
    { "UnableToAllocateColorInfo", 526 },
    { "UnableToAllocateDashPattern", 527 },
    { "UnableToAllocateDelegateInfo", 528 },
    { "UnableToAllocateDerivatives", 529 },
    { "UnableToAllocateDrawContext", 530 },
    { "UnableToAllocateDrawInfo", 531 },
    { "UnableToAllocateGammaMap", 532 },
    { "UnableToAllocateImage", 533 },
    { "UnableToAllocateImagePixels", 534 },
    { "UnableToAllocateLogInfo", 535 },
    { "UnableToAllocateMagicInfo", 536 },
    { "UnableToAllocateMagickInfo", 537 },
    { "UnableToAllocateModuleInfo", 538 },
    { "UnableToAllocateMontageInfo", 539 },
    { "UnableToAllocateQuantizeInfo", 540 },
    { "UnableToAllocateRegistryInfo", 541 },
    { "UnableToAllocateSemaphoreInfo", 542 },
    { "UnableToAllocateString", 543 },
    { "UnableToAllocateTypeInfo", 544 },
    { "UnableToAnimateImageSequence", 545 },
    { "UnableToCloneBlobInfo", 546 },
    { "UnableToCloneCacheInfo", 547 },
    { "UnableToCloneImage", 548 },
    { "UnableToCloneImageInfo", 549 },
    { "UnableToConcatenateString", 550 },
    { "UnableToConvertText", 551 },
    { "UnableToCreateColormap", 552 },
    { "UnableToDisplayImage", 553 },
    { "UnableToEscapeString", 554 },
    { "UnableToInterpretMSLImage", 555 },
    { "MemoryAllocationFailed", 556 },
    { "ImageDoesNotContainTheStreamGeometry", 557 },
    { "NoStreamHandlerIsDefined", 558 },
    { "PixelCacheIsNotOpen", 559 },
    { "UnableToAcquirePixelStream", 560 },
    { "UnableToSetPixelStream", 561 },
    { "UnableToSyncPixelStream", 562 },
    { "Default", 563 },
    { "Default", 564 },
    { "FontSubstitutionRequired", 565 },
    { "UnableToGetTypeMetrics", 566 },
    { "UnableToInitializeFreetypeLibrary", 567 },
    { "UnableToReadFont", 568 },
    { "UnrecognizedFontEncoding", 569 },
    { "Default", 570 },
    { "Default", 571 },
    { "ColorIsNotKnownToServer", 572 },
    { "NoWindowWithSpecifiedIDExists", 573 },
    { "StandardColormapIsNotInitialized", 574 },
    { "UnableToConnectToRemoteDisplay", 575 },
    { "UnableToCreateBitmap", 576 },
    { "UnableToCreateColormap", 577 },
    { "UnableToCreatePixmap", 578 },
    { "UnableToCreateProperty", 579 },
    { "UnableToCreateStandardColormap", 580 },
    { "UnableToDisplayImageInfo", 581 },
    { "UnableToGetProperty", 582 },
    { "UnableToGetStandardColormap", 583 },
    { "UnableToGetVisual", 584 },
    { "UnableToGrabMouse", 585 },
    { "UnableToLoadFont", 586 },
    { "UnableToMatchVisualToStandardColormap", 587 },
    { "UnableToOpenXServer", 588 },
    { "UnableToReadXAttributes", 589 },
    { "UnableToReadXWindowImage", 590 },
    { "UnrecognizedColormapType", 591 },
    { "UnrecognizedGravityType", 592 },
    { "UnrecognizedVisualSpecifier", 593 },
    { "UnableToAllocateXHints", 594 },
    { "UnableToCreateCursor", 595 },
    { "UnableToCreateGraphicContext", 596 },
    { "UnableToCreateStandardColormap", 597 },
    { "UnableToCreateTextProperty", 598 },
    { "UnableToCreateXImage", 599 },
    { "UnableToCreateXPixmap", 600 },
    { "UnableToCreateXWindow", 601 },
    { "UnableToDisplayImage", 602 },
    { "UnableToDitherImage", 603 },
    { "UnableToGetPixelInfo", 604 },
    { "UnableToGetVisual", 605 },
    { "UnableToLoadFont", 606 },
    { "UnableToMakeXWindow", 607 },
    { "UnableToOpenXServer", 608 },
    { "UnableToViewFonts", 609 },
    { "UnableToGetVisual", 610 },
    { "UsingDefaultVisual", 611 },
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
    "image colorspace differs",
    "image opacity differs",
    "Image sequence is required",
    "image size differs",
    "left and right image sizes differ",
    "no images were found",
    "no images were loaded",
    "no [LOCALE] image attribute",
    "too many cluster",
    "unable to append image",
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
