#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 618

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
#define MGK_CoderErrorNotASupportedImageFile 54
#define MGK_CoderErrorNumberOfImagesIsNotSupported 55
#define MGK_CoderErrorOnlyContinuousTonePictureSupported 56
#define MGK_CoderErrorOnlyLevelZerofilesSupported 57
#define MGK_CoderErrorPNGCompressionNotSupported 58
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
#define MGK_CorruptImageErrorNotABMPImageFile 108
#define MGK_CorruptImageErrorNotACUTImageFile 109
#define MGK_CorruptImageErrorNotADCMImageFile 110
#define MGK_CorruptImageErrorNotADCXImageFile 111
#define MGK_CorruptImageErrorNotADIBImageFile 112
#define MGK_CorruptImageErrorNotADPXImageFile 113
#define MGK_CorruptImageErrorNotAGIFImageFile 114
#define MGK_CorruptImageErrorNotAJNGImageFile 115
#define MGK_CorruptImageErrorNotAMATLABImageFile 116
#define MGK_CorruptImageErrorNotAMNGImageFile 117
#define MGK_CorruptImageErrorNotAMTVImageFile 118
#define MGK_CorruptImageErrorNotAnARTImageFile 119
#define MGK_CorruptImageErrorNotAnAVIImageFile 120
#define MGK_CorruptImageErrorNotAnAVSImageFile 121
#define MGK_CorruptImageErrorNotAnEMFImageFile 122
#define MGK_CorruptImageErrorNotAnICOImageFile 123
#define MGK_CorruptImageErrorNotAOTBImageFile 124
#define MGK_CorruptImageErrorNotAPCDImageFile 125
#define MGK_CorruptImageErrorNotAPCXImageFile 126
#define MGK_CorruptImageErrorNotAPDBImageFile 127
#define MGK_CorruptImageErrorNotAPICTImageFile 128
#define MGK_CorruptImageErrorNotAPIXImageFile 129
#define MGK_CorruptImageErrorNotAPNGImageFile 130
#define MGK_CorruptImageErrorNotAPNMImageFile 131
#define MGK_CorruptImageErrorNotAPSDImageFile 132
#define MGK_CorruptImageErrorNotAPWPImageFile 133
#define MGK_CorruptImageErrorNotARLEImageFile 134
#define MGK_CorruptImageErrorNotASCTImageFile 135
#define MGK_CorruptImageErrorNotASFWImageFile 136
#define MGK_CorruptImageErrorNotASGIImageFile 137
#define MGK_CorruptImageErrorNotASUNImageFile 138
#define MGK_CorruptImageErrorNotATGAImageFile 139
#define MGK_CorruptImageErrorNotATIMImageFile 140
#define MGK_CorruptImageErrorNotATOPOLImageFile 141
#define MGK_CorruptImageErrorNotAVIFFImageFile 142
#define MGK_CorruptImageErrorNotAWBMPImageFile 143
#define MGK_CorruptImageErrorNotAWPGImageFile 144
#define MGK_CorruptImageErrorNotAXBMImageFile 145
#define MGK_CorruptImageErrorNotAXCFImageFile 146
#define MGK_CorruptImageErrorNotAXPMImageFile 147
#define MGK_CorruptImageErrorNotEnoughTiles 148
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 149
#define MGK_CorruptImageErrorTooMuchImageDataInFile 150
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 151
#define MGK_CorruptImageErrorUnableToReadColorProfile 152
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 153
#define MGK_CorruptImageErrorUnableToReadGenericProfile 154
#define MGK_CorruptImageErrorUnableToReadImageData 155
#define MGK_CorruptImageErrorUnableToReadImageHeader 156
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 157
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 158
#define MGK_CorruptImageErrorUnableToReadSubImageData 159
#define MGK_CorruptImageErrorUnableToReadVIDImage 160
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 161
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 162
#define MGK_CorruptImageErrorUnableToUncompressImage 163
#define MGK_CorruptImageErrorUnexpectedEndOfFile 164
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 165
#define MGK_CorruptImageErrorUnknownPatternType 166
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 167
#define MGK_CorruptImageErrorUnrecognizedImageCompression 168
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 169
#define MGK_CorruptImageFatalErrorUnableToPersistKey 170
#define MGK_CorruptImageWarningCompressionNotValid 171
#define MGK_CorruptImageWarningImproperImageHeader 172
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 173
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 174
#define MGK_CorruptImageWarningSkipToSyncByte 175
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 176
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 177
#define MGK_CorruptImageWarningUnrecognizedImageCompression 178
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 179
#define MGK_DelegateErrorDelegateFailed 180
#define MGK_DelegateErrorFailedToComputeOutputSize 181
#define MGK_DelegateErrorFailedToRenderFile 182
#define MGK_DelegateErrorFailedToScanFile 183
#define MGK_DelegateErrorNoTagFound 184
#define MGK_DelegateErrorPostscriptDelegateFailed 185
#define MGK_DelegateErrorUnableToCreateImage 186
#define MGK_DelegateErrorUnableToCreateImageComponent 187
#define MGK_DelegateErrorUnableToDecodeImageFile 188
#define MGK_DelegateErrorUnableToEncodeImageFile 189
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 190
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 191
#define MGK_DelegateErrorUnableToManageJP2Stream 192
#define MGK_DelegateErrorUnableToWriteSVGFormat 193
#define MGK_DelegateFatalErrorDefault 194
#define MGK_DelegateWarningDefault 195
#define MGK_DrawErrorAlreadyPushingPatternDefinition 196
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 197
#define MGK_DrawErrorUnableToPrint 198
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 199
#define MGK_DrawFatalErrorDefault 200
#define MGK_DrawWarningNotARelativeURL 201
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 202
#define MGK_DrawWarningURLNotFound 203
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 204
#define MGK_FileOpenErrorUnableToOpenFile 205
#define MGK_FileOpenErrorUnableToWriteFile 206
#define MGK_FileOpenFatalErrorDefault 207
#define MGK_FileOpenWarningDefault 208
#define MGK_ImageErrorAngleIsDiscontinuous 209
#define MGK_ImageErrorColorspaceColorProfileMismatch 210
#define MGK_ImageErrorImageColorspaceDiffers 211
#define MGK_ImageErrorImageOpacityDiffers 212
#define MGK_ImageErrorImageSequenceIsRequired 213
#define MGK_ImageErrorImageSizeDiffers 214
#define MGK_ImageErrorInvalidColormapIndex 215
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 216
#define MGK_ImageErrorNoImagesWereFound 217
#define MGK_ImageErrorNoImagesWereLoaded 218
#define MGK_ImageErrorNoLocaleImageAttribute 219
#define MGK_ImageErrorTooManyClusters 220
#define MGK_ImageErrorUnableToAppendImage 221
#define MGK_ImageErrorUnableToAssignProfile 222
#define MGK_ImageErrorUnableToAverageImage 223
#define MGK_ImageErrorUnableToCoalesceImage 224
#define MGK_ImageErrorUnableToCompareImages 225
#define MGK_ImageErrorUnableToCreateImageMosaic 226
#define MGK_ImageErrorUnableToCreateStereoImage 227
#define MGK_ImageErrorUnableToDeconstructImageSequence 228
#define MGK_ImageErrorUnableToFlattenImage 229
#define MGK_ImageErrorUnableToResizeImage 230
#define MGK_ImageErrorUnableToSegmentImage 231
#define MGK_ImageErrorUnableToSetClipMask 232
#define MGK_ImageErrorUnableToShearImage 233
#define MGK_ImageErrorWidthOrHeightExceedsLimit 234
#define MGK_ImageFatalErrorUnableToPersistKey 235
#define MGK_ImageWarningDefault 236
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 237
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 238
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 239
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 240
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 241
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 242
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 243
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 244
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 245
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 246
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 247
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 248
#define MGK_MissingDelegateFatalErrorDefault 249
#define MGK_MissingDelegateWarningDefault 250
#define MGK_ModuleErrorFailedToCloseModule 251
#define MGK_ModuleErrorFailedToFindSymbol 252
#define MGK_ModuleErrorUnableToLoadModule 253
#define MGK_ModuleErrorUnableToRegisterImageFormat 254
#define MGK_ModuleErrorUnrecognizedModule 255
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 256
#define MGK_ModuleWarningDefault 257
#define MGK_OptionErrorBevelWidthIsNegative 258
#define MGK_OptionErrorColorSeparatedImageRequired 259
#define MGK_OptionErrorFrameIsLessThanImageSize 260
#define MGK_OptionErrorGeometryDimensionsAreZero 261
#define MGK_OptionErrorGeometryDoesNotContainImage 262
#define MGK_OptionErrorImagesAreNotTheSameSize 263
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 264
#define MGK_OptionErrorImageSmallerThanKernelWidth 265
#define MGK_OptionErrorImageSmallerThanRadius 266
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 267
#define MGK_OptionErrorInputImagesAlreadySpecified 268
#define MGK_OptionErrorKernelRadiusIsTooSmall 269
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 270
#define MGK_OptionErrorMissingAffineMatrix 271
#define MGK_OptionErrorMissingAmount 272
#define MGK_OptionErrorMissingAmplitude 273
#define MGK_OptionErrorMissingAnImageFilename 274
#define MGK_OptionErrorMissingArgument 275
#define MGK_OptionErrorMissingAzimuth 276
#define MGK_OptionErrorMissingBackgroundColor 277
#define MGK_OptionErrorMissingBevelWidth 278
#define MGK_OptionErrorMissingBorderColor 279
#define MGK_OptionErrorMissingClippingPathName 280
#define MGK_OptionErrorMissingCoefficients 281
#define MGK_OptionErrorMissingColorizeValue 282
#define MGK_OptionErrorMissingColors 283
#define MGK_OptionErrorMissingComment 284
#define MGK_OptionErrorMissingCompressionQuality 285
#define MGK_OptionErrorMissingCycleAmount 286
#define MGK_OptionErrorMissingDelay 287
#define MGK_OptionErrorMissingDisposeMethod 288
#define MGK_OptionErrorMissingDissolveValue 289
#define MGK_OptionErrorMissingDistance 290
#define MGK_OptionErrorMissingDrawingPrimitive 291
#define MGK_OptionErrorMissingEnodingType 292
#define MGK_OptionErrorMissingEventMask 293
#define MGK_OptionErrorMissingFilename 294
#define MGK_OptionErrorMissingFillColor 295
#define MGK_OptionErrorMissingFontName 296
#define MGK_OptionErrorMissingFormatSpecifier 297
#define MGK_OptionErrorMissingGeometry 298
#define MGK_OptionErrorMissingImageDepth 299
#define MGK_OptionErrorMissingImageFrames 300
#define MGK_OptionErrorMissingImageGamma 301
#define MGK_OptionErrorMissingImageIterations 302
#define MGK_OptionErrorMissingImageLabel 303
#define MGK_OptionErrorMissingImageProfile 304
#define MGK_OptionErrorMissingImageRotation 305
#define MGK_OptionErrorMissingImageScene 306
#define MGK_OptionErrorMissingImageType 307
#define MGK_OptionErrorMissingImplodeAmount 308
#define MGK_OptionErrorMissingListName 309
#define MGK_OptionErrorMissingLogFormat 310
#define MGK_OptionErrorMissingMatrix 311
#define MGK_OptionErrorMissingMatteColor 312
#define MGK_OptionErrorMissingOffset 313
#define MGK_OptionErrorMissingOpaqueColor 314
#define MGK_OptionErrorMissingOrder 315
#define MGK_OptionErrorMissingPageGeometry 316
#define MGK_OptionErrorMissingPaintRadius 317
#define MGK_OptionErrorMissingPassword 318
#define MGK_OptionErrorMissingPoint 319
#define MGK_OptionErrorMissingPointsize 320
#define MGK_OptionErrorMissingResourceLimit 321
#define MGK_OptionErrorMissingResourceType 322
#define MGK_OptionErrorMissingSeedValue 323
#define MGK_OptionErrorMissingServerName 324
#define MGK_OptionErrorMissingSpreadAmount 325
#define MGK_OptionErrorMissingThreshold 326
#define MGK_OptionErrorMissingTile 327
#define MGK_OptionErrorMissingTitle 328
#define MGK_OptionErrorMissingTransparentColor 329
#define MGK_OptionErrorMissingTreeDepth 330
#define MGK_OptionErrorMissingType 331
#define MGK_OptionErrorMissingViewTransform 332
#define MGK_OptionErrorMissingVirtualPixelMethod 333
#define MGK_OptionErrorMissingWidth 334
#define MGK_OptionErrorMustSpecifyAnImageName 335
#define MGK_OptionErrorMustSpecifyImageSize 336
#define MGK_OptionErrorNoBlobDefined 337
#define MGK_OptionErrorNoImagesDefined 338
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 339
#define MGK_OptionErrorNoProfileNameWasGiven 340
#define MGK_OptionErrorNullBlobArgument 341
#define MGK_OptionErrorReferenceImageRequired 342
#define MGK_OptionErrorReferenceIsNotMyType 343
#define MGK_OptionErrorSteganoImageRequired 344
#define MGK_OptionErrorStereoImageRequired 345
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 346
#define MGK_OptionErrorUnableToAddOrRemoveProfile 347
#define MGK_OptionErrorUnableToAllocateICCProfile 348
#define MGK_OptionErrorUnableToAverageImageSequence 349
#define MGK_OptionErrorUnableToBlurImage 350
#define MGK_OptionErrorUnableToChopImage 351
#define MGK_OptionErrorUnableToConstituteImage 352
#define MGK_OptionErrorUnableToConvolveImage 353
#define MGK_OptionErrorUnableToEdgeImage 354
#define MGK_OptionErrorUnableToEqualizeImage 355
#define MGK_OptionErrorUnableToFilterImage 356
#define MGK_OptionErrorUnableToFormatImageMetadata 357
#define MGK_OptionErrorUnableToFrameImage 358
#define MGK_OptionErrorUnableToOilPaintImage 359
#define MGK_OptionErrorUnableToPaintImage 360
#define MGK_OptionErrorUnableToRaiseImage 361
#define MGK_OptionErrorUnableToSharpenImage 362
#define MGK_OptionErrorUnableToThresholdImage 363
#define MGK_OptionErrorUnableToWaveImage 364
#define MGK_OptionErrorUnrecognizedAttribute 365
#define MGK_OptionErrorUnrecognizedChannelType 366
#define MGK_OptionErrorUnrecognizedColor 367
#define MGK_OptionErrorUnrecognizedColormapType 368
#define MGK_OptionErrorUnrecognizedColorspace 369
#define MGK_OptionErrorUnrecognizedComposeOperator 370
#define MGK_OptionErrorUnrecognizedDisposeMethod 371
#define MGK_OptionErrorUnrecognizedElement 372
#define MGK_OptionErrorUnrecognizedEndianType 373
#define MGK_OptionErrorUnrecognizedGravityType 374
#define MGK_OptionErrorUnrecognizedImageCompression 375
#define MGK_OptionErrorUnrecognizedImageFilter 376
#define MGK_OptionErrorUnrecognizedImageFormat 377
#define MGK_OptionErrorUnrecognizedImageMode 378
#define MGK_OptionErrorUnrecognizedImageType 379
#define MGK_OptionErrorUnrecognizedIntentType 380
#define MGK_OptionErrorUnrecognizedInterlaceType 381
#define MGK_OptionErrorUnrecognizedListType 382
#define MGK_OptionErrorUnrecognizedModeType 383
#define MGK_OptionErrorUnrecognizedNoiseType 384
#define MGK_OptionErrorUnrecognizedOption 385
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 386
#define MGK_OptionErrorUnrecognizedPixelMap 387
#define MGK_OptionErrorUnrecognizedPreviewType 388
#define MGK_OptionErrorUnrecognizedResourceType 389
#define MGK_OptionErrorUnrecognizedType 390
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 391
#define MGK_OptionErrorUsageError 392
#define MGK_OptionFatalErrorDeprecatedOption 393
#define MGK_OptionFatalErrorInvalidColorspaceType 394
#define MGK_OptionFatalErrorInvalidEndianType 395
#define MGK_OptionFatalErrorInvalidImageType 396
#define MGK_OptionFatalErrorInvalidInterlaceType 397
#define MGK_OptionFatalErrorMissingAnImageFilename 398
#define MGK_OptionFatalErrorMissingBevelWidth 399
#define MGK_OptionFatalErrorMissingBorderColor 400
#define MGK_OptionFatalErrorMissingColor 401
#define MGK_OptionFatalErrorMissingColors 402
#define MGK_OptionFatalErrorMissingCommand 403
#define MGK_OptionFatalErrorMissingComment 404
#define MGK_OptionFatalErrorMissingCompressionQuality 405
#define MGK_OptionFatalErrorMissingDegrees 406
#define MGK_OptionFatalErrorMissingDepth 407
#define MGK_OptionFatalErrorMissingDisposeMethod 408
#define MGK_OptionFatalErrorMissingEncodingType 409
#define MGK_OptionFatalErrorMissingEventMask 410
#define MGK_OptionFatalErrorMissingFactor 411
#define MGK_OptionFatalErrorMissingFilename 412
#define MGK_OptionFatalErrorMissingFontName 413
#define MGK_OptionFatalErrorMissingForeground 414
#define MGK_OptionFatalErrorMissingGeometry 415
#define MGK_OptionFatalErrorMissingIDNameOrRoot 416
#define MGK_OptionFatalErrorMissingImageDepth 417
#define MGK_OptionFatalErrorMissingLabelName 418
#define MGK_OptionFatalErrorMissingLevel 419
#define MGK_OptionFatalErrorMissingLogFormat 420
#define MGK_OptionFatalErrorMissingMapType 421
#define MGK_OptionFatalErrorMissingMethod 422
#define MGK_OptionFatalErrorMissingName 423
#define MGK_OptionFatalErrorMissingPageGeometry 424
#define MGK_OptionFatalErrorMissingPassword 425
#define MGK_OptionFatalErrorMissingQuality 426
#define MGK_OptionFatalErrorMissingResourceLimit 427
#define MGK_OptionFatalErrorMissingResourceType 428
#define MGK_OptionFatalErrorMissingSceneNumber 429
#define MGK_OptionFatalErrorMissingSeconds 430
#define MGK_OptionFatalErrorMissingServerName 431
#define MGK_OptionFatalErrorMissingSize 432
#define MGK_OptionFatalErrorMissingSnaps 433
#define MGK_OptionFatalErrorMissingThreshold 434
#define MGK_OptionFatalErrorMissingTitle 435
#define MGK_OptionFatalErrorMissingType 436
#define MGK_OptionFatalErrorMissingValue 437
#define MGK_OptionFatalErrorMissingVirtualPixelMethod 438
#define MGK_OptionFatalErrorMissingVisualClass 439
#define MGK_OptionFatalErrorMissingWidth 440
#define MGK_OptionFatalErrorNoImagesWereLoaded 441
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 442
#define MGK_OptionFatalErrorUnableToOpenXServer 443
#define MGK_OptionFatalErrorUnableToPersistKey 444
#define MGK_OptionFatalErrorUnrecognizedColormapType 445
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 446
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 447
#define MGK_OptionFatalErrorUnrecognizedEndianType 448
#define MGK_OptionFatalErrorUnrecognizedFilterType 449
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 450
#define MGK_OptionFatalErrorUnrecognizedImageType 451
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 452
#define MGK_OptionFatalErrorUnrecognizedOption 453
#define MGK_OptionFatalErrorUnrecognizedResourceType 454
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 455
#define MGK_OptionWarningUnrecognizedColor 456
#define MGK_RegistryErrorImageExpected 457
#define MGK_RegistryErrorImageInfoExpected 458
#define MGK_RegistryErrorStructureSizeMismatch 459
#define MGK_RegistryErrorUnableToGetRegistryID 460
#define MGK_RegistryErrorUnableToLocateImage 461
#define MGK_RegistryErrorUnableToSetRegistry 462
#define MGK_RegistryFatalErrorDefault 463
#define MGK_RegistryWarningDefault 464
#define MGK_ResourceLimitErrorCacheResourcesExhausted 465
#define MGK_ResourceLimitErrorMemoryAllocationFailed 466
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 467
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 468
#define MGK_ResourceLimitErrorUnableToAddColorProfile 469
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 470
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 471
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 472
#define MGK_ResourceLimitErrorUnableToAllocateColormap 473
#define MGK_ResourceLimitErrorUnableToAllocateString 474
#define MGK_ResourceLimitErrorUnableToAnnotateImage 475
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 476
#define MGK_ResourceLimitErrorUnableToCloneImage 477
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 478
#define MGK_ResourceLimitErrorUnableToConstituteImage 479
#define MGK_ResourceLimitErrorUnableToConvertFont 480
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 481
#define MGK_ResourceLimitErrorUnableToCreateColormap 482
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 483
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 484
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 485
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 486
#define MGK_ResourceLimitErrorUnableToCreateXWindow 487
#define MGK_ResourceLimitErrorUnableToCropImage 488
#define MGK_ResourceLimitErrorUnableToDespeckleImage 489
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 490
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 491
#define MGK_ResourceLimitErrorUnableToDitherImage 492
#define MGK_ResourceLimitErrorUnableToDrawOnImage 493
#define MGK_ResourceLimitErrorUnableToEdgeImage 494
#define MGK_ResourceLimitErrorUnableToEmbossImage 495
#define MGK_ResourceLimitErrorUnableToEnhanceImage 496
#define MGK_ResourceLimitErrorUnableToFloodfillImage 497
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 498
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 499
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 500
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 501
#define MGK_ResourceLimitErrorUnableToLevelImage 502
#define MGK_ResourceLimitErrorUnableToMagnifyImage 503
#define MGK_ResourceLimitErrorUnableToManageColor 504
#define MGK_ResourceLimitErrorUnableToMapImage 505
#define MGK_ResourceLimitErrorUnableToMapImageSequence 506
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 507
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 508
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 509
#define MGK_ResourceLimitErrorUnableToNormalizeImage 510
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 511
#define MGK_ResourceLimitErrorUnableToQuantizeImage 512
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 513
#define MGK_ResourceLimitErrorUnableToReadTextChunk 514
#define MGK_ResourceLimitErrorUnableToReadXImage 515
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 516
#define MGK_ResourceLimitErrorUnableToResizeImage 517
#define MGK_ResourceLimitErrorUnableToRotateImage 518
#define MGK_ResourceLimitErrorUnableToSampleImage 519
#define MGK_ResourceLimitErrorUnableToScaleImage 520
#define MGK_ResourceLimitErrorUnableToSelectImage 521
#define MGK_ResourceLimitErrorUnableToSharpenImage 522
#define MGK_ResourceLimitErrorUnableToShaveImage 523
#define MGK_ResourceLimitErrorUnableToShearImage 524
#define MGK_ResourceLimitErrorUnableToSortImageColormap 525
#define MGK_ResourceLimitErrorUnableToThresholdImage 526
#define MGK_ResourceLimitErrorUnableToTransformColorspace 527
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 528
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 529
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 530
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 531
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 532
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 533
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 534
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 535
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 536
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 537
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 538
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 539
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 540
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 541
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 542
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 543
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 544
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 545
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 546
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 547
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 548
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 549
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 550
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 551
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 552
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 553
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 554
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 555
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 556
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 557
#define MGK_ResourceLimitFatalErrorUnableToConvertText 558
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 559
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 560
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 561
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 562
#define MGK_ResourceLimitWarningMemoryAllocationFailed 563
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 564
#define MGK_StreamErrorNoStreamHandlerIsDefined 565
#define MGK_StreamErrorPixelCacheIsNotOpen 566
#define MGK_StreamErrorUnableToAcquirePixelStream 567
#define MGK_StreamErrorUnableToSetPixelStream 568
#define MGK_StreamErrorUnableToSyncPixelStream 569
#define MGK_StreamFatalErrorDefault 570
#define MGK_StreamWarningDefault 571
#define MGK_TypeErrorFontSubstitutionRequired 572
#define MGK_TypeErrorUnableToGetTypeMetrics 573
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 574
#define MGK_TypeErrorUnableToReadFont 575
#define MGK_TypeErrorUnrecognizedFontEncoding 576
#define MGK_TypeFatalErrorDefault 577
#define MGK_TypeWarningDefault 578
#define MGK_XServerErrorColorIsNotKnownToServer 579
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 580
#define MGK_XServerErrorStandardColormapIsNotInitialized 581
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 582
#define MGK_XServerErrorUnableToCreateBitmap 583
#define MGK_XServerErrorUnableToCreateColormap 584
#define MGK_XServerErrorUnableToCreatePixmap 585
#define MGK_XServerErrorUnableToCreateProperty 586
#define MGK_XServerErrorUnableToCreateStandardColormap 587
#define MGK_XServerErrorUnableToDisplayImageInfo 588
#define MGK_XServerErrorUnableToGetProperty 589
#define MGK_XServerErrorUnableToGetStandardColormap 590
#define MGK_XServerErrorUnableToGetVisual 591
#define MGK_XServerErrorUnableToGrabMouse 592
#define MGK_XServerErrorUnableToLoadFont 593
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 594
#define MGK_XServerErrorUnableToOpenXServer 595
#define MGK_XServerErrorUnableToReadXAttributes 596
#define MGK_XServerErrorUnableToReadXWindowImage 597
#define MGK_XServerErrorUnrecognizedColormapType 598
#define MGK_XServerErrorUnrecognizedGravityType 599
#define MGK_XServerErrorUnrecognizedVisualSpecifier 600
#define MGK_XServerFatalErrorUnableToAllocateXHints 601
#define MGK_XServerFatalErrorUnableToCreateCursor 602
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 603
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 604
#define MGK_XServerFatalErrorUnableToCreateTextProperty 605
#define MGK_XServerFatalErrorUnableToCreateXImage 606
#define MGK_XServerFatalErrorUnableToCreateXPixmap 607
#define MGK_XServerFatalErrorUnableToCreateXWindow 608
#define MGK_XServerFatalErrorUnableToDisplayImage 609
#define MGK_XServerFatalErrorUnableToDitherImage 610
#define MGK_XServerFatalErrorUnableToGetPixelInfo 611
#define MGK_XServerFatalErrorUnableToGetVisual 612
#define MGK_XServerFatalErrorUnableToLoadFont 613
#define MGK_XServerFatalErrorUnableToMakeXWindow 614
#define MGK_XServerFatalErrorUnableToOpenXServer 615
#define MGK_XServerFatalErrorUnableToViewFonts 616
#define MGK_XServerWarningUnableToGetVisual 617
#define MGK_XServerWarningUsingDefaultVisual 618

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
    { "Corrupt/Image/FatalError", 169, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 170, CorruptImageWarning },
    { "Delegate/Error", 179, DelegateError },
    { "Delegate/FatalError", 193, DelegateFatalError },
    { "Delegate/Warning", 194, DelegateWarning },
    { "Draw/Error", 195, DrawError },
    { "Draw/FatalError", 199, DrawFatalError },
    { "Draw/Warning", 200, DrawWarning },
    { "File/Open/Error", 203, FileOpenError },
    { "File/Open/FatalError", 206, FileOpenFatalError },
    { "File/Open/Warning", 207, FileOpenWarning },
    { "Image/Error", 208, ImageError },
    { "Image/FatalError", 234, ImageFatalError },
    { "Image/Warning", 235, ImageWarning },
    { "Missing/Delegate/Error", 236, MissingDelegateError },
    { "Missing/Delegate/FatalError", 248, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 249, MissingDelegateWarning },
    { "Module/Error", 250, ModuleError },
    { "Module/FatalError", 255, ModuleFatalError },
    { "Module/Warning", 256, ModuleWarning },
    { "Option/Error", 257, OptionError },
    { "Option/FatalError", 392, OptionFatalError },
    { "Option/Warning", 455, OptionWarning },
    { "Registry/Error", 456, RegistryError },
    { "Registry/FatalError", 462, RegistryFatalError },
    { "Registry/Warning", 463, RegistryWarning },
    { "Resource/Limit/Error", 464, ResourceLimitError },
    { "Resource/Limit/FatalError", 527, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 562, ResourceLimitWarning },
    { "Stream/Error", 563, StreamError },
    { "Stream/FatalError", 569, StreamFatalError },
    { "Stream/Warning", 570, StreamWarning },
    { "Type/Error", 571, TypeError },
    { "Type/FatalError", 576, TypeFatalError },
    { "Type/Warning", 577, TypeWarning },
    { "XServer/Error", 578, XServerError },
    { "XServer/FatalError", 600, XServerFatalError },
    { "XServer/Warning", 616, XServerWarning },
    { 0, 618, 0 }
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
    { "NotASupportedImageFile", 54 },
    { "NumberOfImagesIsNotSupported", 55 },
    { "OnlyContinuousTonePictureSupported", 56 },
    { "OnlyLevelZerofilesSupported", 57 },
    { "PNGCompressionNotSupported", 58 },
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
    { "NotABMPImageFile", 108 },
    { "NotACUTImageFile", 109 },
    { "NotADCMImageFile", 110 },
    { "NotADCXImageFile", 111 },
    { "NotADIBImageFile", 112 },
    { "NotADPXImageFile", 113 },
    { "NotAGIFImageFile", 114 },
    { "NotAJNGImageFile", 115 },
    { "NotAMATLABImageFile", 116 },
    { "NotAMNGImageFile", 117 },
    { "NotAMTVImageFile", 118 },
    { "NotAnARTImageFile", 119 },
    { "NotAnAVIImageFile", 120 },
    { "NotAnAVSImageFile", 121 },
    { "NotAnEMFImageFile", 122 },
    { "NotAnICOImageFile", 123 },
    { "NotAOTBImageFile", 124 },
    { "NotAPCDImageFile", 125 },
    { "NotAPCXImageFile", 126 },
    { "NotAPDBImageFile", 127 },
    { "NotAPICTImageFile", 128 },
    { "NotAPIXImageFile", 129 },
    { "NotAPNGImageFile", 130 },
    { "NotAPNMImageFile", 131 },
    { "NotAPSDImageFile", 132 },
    { "NotAPWPImageFile", 133 },
    { "NotARLEImageFile", 134 },
    { "NotASCTImageFile", 135 },
    { "NotASFWImageFile", 136 },
    { "NotASGIImageFile", 137 },
    { "NotASUNImageFile", 138 },
    { "NotATGAImageFile", 139 },
    { "NotATIMImageFile", 140 },
    { "NotATOPOLImageFile", 141 },
    { "NotAVIFFImageFile", 142 },
    { "NotAWBMPImageFile", 143 },
    { "NotAWPGImageFile", 144 },
    { "NotAXBMImageFile", 145 },
    { "NotAXCFImageFile", 146 },
    { "NotAXPMImageFile", 147 },
    { "NotEnoughTiles", 148 },
    { "StaticPlanesValueNotEqualToOne", 149 },
    { "TooMuchImageDataInFile", 150 },
    { "UnableToReadColormapFromDumpFile", 151 },
    { "UnableToReadColorProfile", 152 },
    { "UnableToReadExtensionBlock", 153 },
    { "UnableToReadGenericProfile", 154 },
    { "UnableToReadImageData", 155 },
    { "UnableToReadImageHeader", 156 },
    { "UnableToReadIPTCProfile", 157 },
    { "UnableToReadPixmapFromDumpFile", 158 },
    { "UnableToReadSubImageData", 159 },
    { "UnableToReadVIDImage", 160 },
    { "UnableToReadWindowNameFromDumpFile", 161 },
    { "UnableToRunlengthDecodeImage", 162 },
    { "UnableToUncompressImage", 163 },
    { "UnexpectedEndOfFile", 164 },
    { "UnexpectedSamplingFactor", 165 },
    { "UnknownPatternType", 166 },
    { "UnrecognizedBitsPerPixel", 167 },
    { "UnrecognizedImageCompression", 168 },
    { "UnrecognizedXWDHeader", 169 },
    { "UnableToPersistKey", 170 },
    { "CompressionNotValid", 171 },
    { "ImproperImageHeader", 172 },
    { "NegativeOrZeroImageSize", 173 },
    { "NonOS2HeaderSizeError", 174 },
    { "SkipToSyncByte", 175 },
    { "StaticPlanesValueNotEqualToOne", 176 },
    { "UnrecognizedBitsPerPixel", 177 },
    { "UnrecognizedImageCompression", 178 },
    { "UnrecognizedNumberOfColors", 179 },
    { "DelegateFailed", 180 },
    { "FailedToComputeOutputSize", 181 },
    { "FailedToRenderFile", 182 },
    { "FailedToScanFile", 183 },
    { "NoTagFound", 184 },
    { "PostscriptDelegateFailed", 185 },
    { "UnableToCreateImage", 186 },
    { "UnableToCreateImageComponent", 187 },
    { "UnableToDecodeImageFile", 188 },
    { "UnableToEncodeImageFile", 189 },
    { "UnableToInitializeFPXLibrary", 190 },
    { "UnableToInitializeWMFLibrary", 191 },
    { "UnableToManageJP2Stream", 192 },
    { "UnableToWriteSVGFormat", 193 },
    { "Default", 194 },
    { "Default", 195 },
    { "AlreadyPushingPatternDefinition", 196 },
    { "NonconformingDrawingPrimitiveDefinition", 197 },
    { "UnableToPrint", 198 },
    { "UnbalancedGraphicContextPushPop", 199 },
    { "Default", 200 },
    { "NotARelativeURL", 201 },
    { "NotCurrentlyPushingPatternDefinition", 202 },
    { "URLNotFound", 203 },
    { "UnableToCreateTemporaryFile", 204 },
    { "UnableToOpenFile", 205 },
    { "UnableToWriteFile", 206 },
    { "Default", 207 },
    { "Default", 208 },
    { "AngleIsDiscontinuous", 209 },
    { "ColorspaceColorProfileMismatch", 210 },
    { "ImageColorspaceDiffers", 211 },
    { "ImageOpacityDiffers", 212 },
    { "ImageSequenceIsRequired", 213 },
    { "ImageSizeDiffers", 214 },
    { "InvalidColormapIndex", 215 },
    { "LeftAndRightImageSizesDiffer", 216 },
    { "NoImagesWereFound", 217 },
    { "NoImagesWereLoaded", 218 },
    { "NoLocaleImageAttribute", 219 },
    { "TooManyClusters", 220 },
    { "UnableToAppendImage", 221 },
    { "UnableToAssignProfile", 222 },
    { "UnableToAverageImage", 223 },
    { "UnableToCoalesceImage", 224 },
    { "UnableToCompareImages", 225 },
    { "UnableToCreateImageMosaic", 226 },
    { "UnableToCreateStereoImage", 227 },
    { "UnableToDeconstructImageSequence", 228 },
    { "UnableToFlattenImage", 229 },
    { "UnableToResizeImage", 230 },
    { "UnableToSegmentImage", 231 },
    { "UnableToSetClipMask", 232 },
    { "UnableToShearImage", 233 },
    { "WidthOrHeightExceedsLimit", 234 },
    { "UnableToPersistKey", 235 },
    { "Default", 236 },
    { "DPSLibraryIsNotAvailable", 237 },
    { "FPXLibraryIsNotAvailable", 238 },
    { "FreeTypeLibraryIsNotAvailable", 239 },
    { "JPEGLibraryIsNotAvailable", 240 },
    { "LCMSLibraryIsNotAvailable", 241 },
    { "LZWEncodingNotEnabled", 242 },
    { "NoDecodeDelegateForThisImageFormat", 243 },
    { "NoEncodeDelegateForThisImageFormat", 244 },
    { "TIFFLibraryIsNotAvailable", 245 },
    { "XMLLibraryIsNotAvailable", 246 },
    { "XWindowLibraryIsNotAvailable", 247 },
    { "ZipLibraryIsNotAvailable", 248 },
    { "Default", 249 },
    { "Default", 250 },
    { "FailedToCloseModule", 251 },
    { "FailedToFindSymbol", 252 },
    { "UnableToLoadModule", 253 },
    { "UnableToRegisterImageFormat", 254 },
    { "UnrecognizedModule", 255 },
    { "UnableToInitializeModuleLoader", 256 },
    { "Default", 257 },
    { "BevelWidthIsNegative", 258 },
    { "ColorSeparatedImageRequired", 259 },
    { "FrameIsLessThanImageSize", 260 },
    { "GeometryDimensionsAreZero", 261 },
    { "GeometryDoesNotContainImage", 262 },
    { "ImagesAreNotTheSameSize", 263 },
    { "ImageSizeMustExceedBevelWidth", 264 },
    { "ImageSmallerThanKernelWidth", 265 },
    { "ImageSmallerThanRadius", 266 },
    { "ImageWidthsOrHeightsDiffer", 267 },
    { "InputImagesAlreadySpecified", 268 },
    { "KernelRadiusIsTooSmall", 269 },
    { "KernelWidthMustBeAnOddNumber", 270 },
    { "MissingAffineMatrix", 271 },
    { "MissingAmount", 272 },
    { "MissingAmplitude", 273 },
    { "MissingAnImageFilename", 274 },
    { "MissingArgument", 275 },
    { "MissingAzimuth", 276 },
    { "MissingBackgroundColor", 277 },
    { "MissingBevelWidth", 278 },
    { "MissingBorderColor", 279 },
    { "MissingClippingPathName", 280 },
    { "MissingCoefficients", 281 },
    { "MissingColorizeValue", 282 },
    { "MissingColors", 283 },
    { "MissingComment", 284 },
    { "MissingCompressionQuality", 285 },
    { "MissingCycleAmount", 286 },
    { "MissingDelay", 287 },
    { "MissingDisposeMethod", 288 },
    { "MissingDissolveValue", 289 },
    { "MissingDistance", 290 },
    { "MissingDrawingPrimitive", 291 },
    { "MissingEnodingType", 292 },
    { "MissingEventMask", 293 },
    { "MissingFilename", 294 },
    { "MissingFillColor", 295 },
    { "MissingFontName", 296 },
    { "MissingFormatSpecifier", 297 },
    { "MissingGeometry", 298 },
    { "MissingImageDepth", 299 },
    { "MissingImageFrames", 300 },
    { "MissingImageGamma", 301 },
    { "MissingImageIterations", 302 },
    { "MissingImageLabel", 303 },
    { "MissingImageProfile", 304 },
    { "MissingImageRotation", 305 },
    { "MissingImageScene", 306 },
    { "MissingImageType", 307 },
    { "MissingImplodeAmount", 308 },
    { "MissingListName", 309 },
    { "MissingLogFormat", 310 },
    { "MissingMatrix", 311 },
    { "MissingMatteColor", 312 },
    { "MissingOffset", 313 },
    { "MissingOpaqueColor", 314 },
    { "MissingOrder", 315 },
    { "MissingPageGeometry", 316 },
    { "MissingPaintRadius", 317 },
    { "MissingPassword", 318 },
    { "MissingPoint", 319 },
    { "MissingPointsize", 320 },
    { "MissingResourceLimit", 321 },
    { "MissingResourceType", 322 },
    { "MissingSeedValue", 323 },
    { "MissingServerName", 324 },
    { "MissingSpreadAmount", 325 },
    { "MissingThreshold", 326 },
    { "MissingTile", 327 },
    { "MissingTitle", 328 },
    { "MissingTransparentColor", 329 },
    { "MissingTreeDepth", 330 },
    { "MissingType", 331 },
    { "MissingViewTransform", 332 },
    { "MissingVirtualPixelMethod", 333 },
    { "MissingWidth", 334 },
    { "MustSpecifyAnImageName", 335 },
    { "MustSpecifyImageSize", 336 },
    { "NoBlobDefined", 337 },
    { "NoImagesDefined", 338 },
    { "NonzeroWidthAndHeightRequired", 339 },
    { "NoProfileNameWasGiven", 340 },
    { "NullBlobArgument", 341 },
    { "ReferenceImageRequired", 342 },
    { "ReferenceIsNotMyType", 343 },
    { "SteganoImageRequired", 344 },
    { "StereoImageRequired", 345 },
    { "SubimageSpecificationReturnsNoImages", 346 },
    { "UnableToAddOrRemoveProfile", 347 },
    { "UnableToAllocateICCProfile", 348 },
    { "UnableToAverageImageSequence", 349 },
    { "UnableToBlurImage", 350 },
    { "UnableToChopImage", 351 },
    { "UnableToConstituteImage", 352 },
    { "UnableToConvolveImage", 353 },
    { "UnableToEdgeImage", 354 },
    { "UnableToEqualizeImage", 355 },
    { "UnableToFilterImage", 356 },
    { "UnableToFormatImageMetadata", 357 },
    { "UnableToFrameImage", 358 },
    { "UnableToOilPaintImage", 359 },
    { "UnableToPaintImage", 360 },
    { "UnableToRaiseImage", 361 },
    { "UnableToSharpenImage", 362 },
    { "UnableToThresholdImage", 363 },
    { "UnableToWaveImage", 364 },
    { "UnrecognizedAttribute", 365 },
    { "UnrecognizedChannelType", 366 },
    { "UnrecognizedColor", 367 },
    { "UnrecognizedColormapType", 368 },
    { "UnrecognizedColorspace", 369 },
    { "UnrecognizedComposeOperator", 370 },
    { "UnrecognizedDisposeMethod", 371 },
    { "UnrecognizedElement", 372 },
    { "UnrecognizedEndianType", 373 },
    { "UnrecognizedGravityType", 374 },
    { "UnrecognizedImageCompression", 375 },
    { "UnrecognizedImageFilter", 376 },
    { "UnrecognizedImageFormat", 377 },
    { "UnrecognizedImageMode", 378 },
    { "UnrecognizedImageType", 379 },
    { "UnrecognizedIntentType", 380 },
    { "UnrecognizedInterlaceType", 381 },
    { "UnrecognizedListType", 382 },
    { "UnrecognizedModeType", 383 },
    { "UnrecognizedNoiseType", 384 },
    { "UnrecognizedOption", 385 },
    { "UnrecognizedPerlMagickMethod", 386 },
    { "UnrecognizedPixelMap", 387 },
    { "UnrecognizedPreviewType", 388 },
    { "UnrecognizedResourceType", 389 },
    { "UnrecognizedType", 390 },
    { "UnrecognizedVirtualPixelMethod", 391 },
    { "UsageError", 392 },
    { "DeprecatedOption", 393 },
    { "InvalidColorspaceType", 394 },
    { "InvalidEndianType", 395 },
    { "InvalidImageType", 396 },
    { "InvalidInterlaceType", 397 },
    { "MissingAnImageFilename", 398 },
    { "MissingBevelWidth", 399 },
    { "MissingBorderColor", 400 },
    { "MissingColor", 401 },
    { "MissingColors", 402 },
    { "MissingCommand", 403 },
    { "MissingComment", 404 },
    { "MissingCompressionQuality", 405 },
    { "MissingDegrees", 406 },
    { "MissingDepth", 407 },
    { "MissingDisposeMethod", 408 },
    { "MissingEncodingType", 409 },
    { "MissingEventMask", 410 },
    { "MissingFactor", 411 },
    { "MissingFilename", 412 },
    { "MissingFontName", 413 },
    { "MissingForeground", 414 },
    { "MissingGeometry", 415 },
    { "MissingIDNameOrRoot", 416 },
    { "MissingImageDepth", 417 },
    { "MissingLabelName", 418 },
    { "MissingLevel", 419 },
    { "MissingLogFormat", 420 },
    { "MissingMapType", 421 },
    { "MissingMethod", 422 },
    { "MissingName", 423 },
    { "MissingPageGeometry", 424 },
    { "MissingPassword", 425 },
    { "MissingQuality", 426 },
    { "MissingResourceLimit", 427 },
    { "MissingResourceType", 428 },
    { "MissingSceneNumber", 429 },
    { "MissingSeconds", 430 },
    { "MissingServerName", 431 },
    { "MissingSize", 432 },
    { "MissingSnaps", 433 },
    { "MissingThreshold", 434 },
    { "MissingTitle", 435 },
    { "MissingType", 436 },
    { "MissingValue", 437 },
    { "MissingVirtualPixelMethod", 438 },
    { "MissingVisualClass", 439 },
    { "MissingWidth", 440 },
    { "NoImagesWereLoaded", 441 },
    { "OptionLengthExceedsLimit", 442 },
    { "UnableToOpenXServer", 443 },
    { "UnableToPersistKey", 444 },
    { "UnrecognizedColormapType", 445 },
    { "UnrecognizedColorspaceType", 446 },
    { "UnrecognizedDisposeMethod", 447 },
    { "UnrecognizedEndianType", 448 },
    { "UnrecognizedFilterType", 449 },
    { "UnrecognizedImageCompressionType", 450 },
    { "UnrecognizedImageType", 451 },
    { "UnrecognizedInterlaceType", 452 },
    { "UnrecognizedOption", 453 },
    { "UnrecognizedResourceType", 454 },
    { "UnrecognizedVirtualPixelMethod", 455 },
    { "UnrecognizedColor", 456 },
    { "ImageExpected", 457 },
    { "ImageInfoExpected", 458 },
    { "StructureSizeMismatch", 459 },
    { "UnableToGetRegistryID", 460 },
    { "UnableToLocateImage", 461 },
    { "UnableToSetRegistry", 462 },
    { "Default", 463 },
    { "Default", 464 },
    { "CacheResourcesExhausted", 465 },
    { "MemoryAllocationFailed", 466 },
    { "NoPixelsDefinedInCache", 467 },
    { "PixelCacheAllocationFailed", 468 },
    { "UnableToAddColorProfile", 469 },
    { "UnableToAddGenericProfile", 470 },
    { "UnableToAddIPTCProfile", 471 },
    { "UnableToAllocateCoefficients", 472 },
    { "UnableToAllocateColormap", 473 },
    { "UnableToAllocateString", 474 },
    { "UnableToAnnotateImage", 475 },
    { "UnableToAverageImageSequence", 476 },
    { "UnableToCloneImage", 477 },
    { "UnableToComputeImageSignature", 478 },
    { "UnableToConstituteImage", 479 },
    { "UnableToConvertFont", 480 },
    { "UnableToConvertStringToTokens", 481 },
    { "UnableToCreateColormap", 482 },
    { "UnableToCreateColorTransform", 483 },
    { "UnableToCreateCommandWidget", 484 },
    { "UnableToCreateImageGroup", 485 },
    { "UnableToCreateImageMontage", 486 },
    { "UnableToCreateXWindow", 487 },
    { "UnableToCropImage", 488 },
    { "UnableToDespeckleImage", 489 },
    { "UnableToDetermineImageClass", 490 },
    { "UnableToDetermineTheNumberOfImageColors", 491 },
    { "UnableToDitherImage", 492 },
    { "UnableToDrawOnImage", 493 },
    { "UnableToEdgeImage", 494 },
    { "UnableToEmbossImage", 495 },
    { "UnableToEnhanceImage", 496 },
    { "UnableToFloodfillImage", 497 },
    { "UnableToGammaCorrectImage", 498 },
    { "UnableToGetBestIconSize", 499 },
    { "UnableToGetFromRegistry", 500 },
    { "UnableToGetPackageInfo", 501 },
    { "UnableToLevelImage", 502 },
    { "UnableToMagnifyImage", 503 },
    { "UnableToManageColor", 504 },
    { "UnableToMapImage", 505 },
    { "UnableToMapImageSequence", 506 },
    { "UnableToMedianFilterImage", 507 },
    { "UnableToMotionBlurImage", 508 },
    { "UnableToNoiseFilterImage", 509 },
    { "UnableToNormalizeImage", 510 },
    { "UnableToOpenColorProfile", 511 },
    { "UnableToQuantizeImage", 512 },
    { "UnableToQuantizeImageSequence", 513 },
    { "UnableToReadTextChunk", 514 },
    { "UnableToReadXImage", 515 },
    { "UnableToReadXServerColormap", 516 },
    { "UnableToResizeImage", 517 },
    { "UnableToRotateImage", 518 },
    { "UnableToSampleImage", 519 },
    { "UnableToScaleImage", 520 },
    { "UnableToSelectImage", 521 },
    { "UnableToSharpenImage", 522 },
    { "UnableToShaveImage", 523 },
    { "UnableToShearImage", 524 },
    { "UnableToSortImageColormap", 525 },
    { "UnableToThresholdImage", 526 },
    { "UnableToTransformColorspace", 527 },
    { "MemoryAllocationFailed", 528 },
    { "UnableToAllocateAscii85Info", 529 },
    { "UnableToAllocateCacheInfo", 530 },
    { "UnableToAllocateCacheView", 531 },
    { "UnableToAllocateColorInfo", 532 },
    { "UnableToAllocateDashPattern", 533 },
    { "UnableToAllocateDelegateInfo", 534 },
    { "UnableToAllocateDerivatives", 535 },
    { "UnableToAllocateDrawContext", 536 },
    { "UnableToAllocateDrawInfo", 537 },
    { "UnableToAllocateDrawingWand", 538 },
    { "UnableToAllocateGammaMap", 539 },
    { "UnableToAllocateImage", 540 },
    { "UnableToAllocateImagePixels", 541 },
    { "UnableToAllocateLogInfo", 542 },
    { "UnableToAllocateMagicInfo", 543 },
    { "UnableToAllocateMagickInfo", 544 },
    { "UnableToAllocateModuleInfo", 545 },
    { "UnableToAllocateMontageInfo", 546 },
    { "UnableToAllocateQuantizeInfo", 547 },
    { "UnableToAllocateRegistryInfo", 548 },
    { "UnableToAllocateSemaphoreInfo", 549 },
    { "UnableToAllocateString", 550 },
    { "UnableToAllocateTypeInfo", 551 },
    { "UnableToAnimateImageSequence", 552 },
    { "UnableToCloneBlobInfo", 553 },
    { "UnableToCloneCacheInfo", 554 },
    { "UnableToCloneImage", 555 },
    { "UnableToCloneImageInfo", 556 },
    { "UnableToConcatenateString", 557 },
    { "UnableToConvertText", 558 },
    { "UnableToCreateColormap", 559 },
    { "UnableToDisplayImage", 560 },
    { "UnableToEscapeString", 561 },
    { "UnableToInterpretMSLImage", 562 },
    { "MemoryAllocationFailed", 563 },
    { "ImageDoesNotContainTheStreamGeometry", 564 },
    { "NoStreamHandlerIsDefined", 565 },
    { "PixelCacheIsNotOpen", 566 },
    { "UnableToAcquirePixelStream", 567 },
    { "UnableToSetPixelStream", 568 },
    { "UnableToSyncPixelStream", 569 },
    { "Default", 570 },
    { "Default", 571 },
    { "FontSubstitutionRequired", 572 },
    { "UnableToGetTypeMetrics", 573 },
    { "UnableToInitializeFreetypeLibrary", 574 },
    { "UnableToReadFont", 575 },
    { "UnrecognizedFontEncoding", 576 },
    { "Default", 577 },
    { "Default", 578 },
    { "ColorIsNotKnownToServer", 579 },
    { "NoWindowWithSpecifiedIDExists", 580 },
    { "StandardColormapIsNotInitialized", 581 },
    { "UnableToConnectToRemoteDisplay", 582 },
    { "UnableToCreateBitmap", 583 },
    { "UnableToCreateColormap", 584 },
    { "UnableToCreatePixmap", 585 },
    { "UnableToCreateProperty", 586 },
    { "UnableToCreateStandardColormap", 587 },
    { "UnableToDisplayImageInfo", 588 },
    { "UnableToGetProperty", 589 },
    { "UnableToGetStandardColormap", 590 },
    { "UnableToGetVisual", 591 },
    { "UnableToGrabMouse", 592 },
    { "UnableToLoadFont", 593 },
    { "UnableToMatchVisualToStandardColormap", 594 },
    { "UnableToOpenXServer", 595 },
    { "UnableToReadXAttributes", 596 },
    { "UnableToReadXWindowImage", 597 },
    { "UnrecognizedColormapType", 598 },
    { "UnrecognizedGravityType", 599 },
    { "UnrecognizedVisualSpecifier", 600 },
    { "UnableToAllocateXHints", 601 },
    { "UnableToCreateCursor", 602 },
    { "UnableToCreateGraphicContext", 603 },
    { "UnableToCreateStandardColormap", 604 },
    { "UnableToCreateTextProperty", 605 },
    { "UnableToCreateXImage", 606 },
    { "UnableToCreateXPixmap", 607 },
    { "UnableToCreateXWindow", 608 },
    { "UnableToDisplayImage", 609 },
    { "UnableToDitherImage", 610 },
    { "UnableToGetPixelInfo", 611 },
    { "UnableToGetVisual", 612 },
    { "UnableToLoadFont", 613 },
    { "UnableToMakeXWindow", 614 },
    { "UnableToOpenXServer", 615 },
    { "UnableToViewFonts", 616 },
    { "UnableToGetVisual", 617 },
    { "UsingDefaultVisual", 618 },
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
    "Missing affine matrix",
    "Missing amount",
    "Missing amplitude",
    "Missing an image filename",
    "Missing argument",
    "Missing azimuth",
    "Missing background color",
    "Missing bevel width",
    "Missing border color",
    "Missing clipping path name",
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
