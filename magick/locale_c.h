#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 617

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
#define MGK_CoderErrorJPEGCompressionNotSupported 38
#define MGK_CoderErrorJPEGEmbeddingFailed 39
#define MGK_CoderErrorLocationTypeIsNotSupported 40
#define MGK_CoderErrorMapStorageTypeIsNotSupported 41
#define MGK_CoderErrorMSBByteOrderNotSupported 42
#define MGK_CoderErrorMultidimensionalMatricesAreNotSupported 43
#define MGK_CoderErrorMultipleRecordListNotSupported 44
#define MGK_CoderErrorNo8BIMDataIsAvailable 45
#define MGK_CoderErrorNoAPP1DataIsAvailable 46
#define MGK_CoderErrorNoBitmapOnClipboard 47
#define MGK_CoderErrorNoColorProfileAvailable 48
#define MGK_CoderErrorNoDataReturned 49
#define MGK_CoderErrorNoImageVectorGraphics 50
#define MGK_CoderErrorNoIPTCInfoWasFound 51
#define MGK_CoderErrorNoIPTCProfileAvailable 52
#define MGK_CoderErrorNotASupportedImageFile 53
#define MGK_CoderErrorNumberOfImagesIsNotSupported 54
#define MGK_CoderErrorOnlyContinuousTonePictureSupported 55
#define MGK_CoderErrorOnlyLevelZerofilesSupported 56
#define MGK_CoderErrorPNGCompressionNotSupported 57
#define MGK_CoderErrorRLECompressionNotSupported 58
#define MGK_CoderErrorUnableToCopyProfile 59
#define MGK_CoderErrorUnableToCreateADC 60
#define MGK_CoderErrorUnableToCreateBitmap 61
#define MGK_CoderErrorUnableToDecompressImage 62
#define MGK_CoderErrorUnableToInitializeFPXLibrary 63
#define MGK_CoderErrorUnableToOpenBlob 64
#define MGK_CoderErrorUnableToReadAspectRatio 65
#define MGK_CoderErrorUnableToReadCIELABImages 66
#define MGK_CoderErrorUnableToReadSummaryInfo 67
#define MGK_CoderErrorUnableToSetAffineMatrix 68
#define MGK_CoderErrorUnableToSetAspectRatio 69
#define MGK_CoderErrorUnableToSetColorTwist 70
#define MGK_CoderErrorUnableToSetContrast 71
#define MGK_CoderErrorUnableToSetFilteringValue 72
#define MGK_CoderErrorUnableToSetImageComments 73
#define MGK_CoderErrorUnableToSetImageTitle 74
#define MGK_CoderErrorUnableToSetJPEGLevel 75
#define MGK_CoderErrorUnableToSetRegionOfInterest 76
#define MGK_CoderErrorUnableToSetSummaryInfo 77
#define MGK_CoderErrorUnableToTranslateText 78
#define MGK_CoderErrorUnableToWriteMPEGParameters 79
#define MGK_CoderErrorUnableToZipCompressImage 80
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 81
#define MGK_CoderErrorZipCompressionNotSupported 82
#define MGK_CoderFatalErrorDefault 83
#define MGK_CoderWarningLosslessToLossyJPEGConversion 84
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 85
#define MGK_ConfigureErrorRegistryKeyLookupFailed 86
#define MGK_ConfigureErrorUnableToAccessConfigureFile 87
#define MGK_ConfigureErrorUnableToAccessFontFile 88
#define MGK_ConfigureErrorUnableToAccessLogFile 89
#define MGK_ConfigureErrorUnableToAccessModuleFile 90
#define MGK_ConfigureFatalErrorDefault 91
#define MGK_ConfigureWarningDefault 92
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 93
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 94
#define MGK_CorruptImageErrorColormapExceeds256Colors 95
#define MGK_CorruptImageErrorCorruptImage 96
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 97
#define MGK_CorruptImageErrorImageTypeNotSupported 98
#define MGK_CorruptImageErrorImproperImageHeader 99
#define MGK_CorruptImageErrorInsufficientImageDataInFile 100
#define MGK_CorruptImageErrorInvalidColormapIndex 101
#define MGK_CorruptImageErrorInvalidFileFormatVersion 102
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 103
#define MGK_CorruptImageErrorMissingImageChannel 104
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 105
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 106
#define MGK_CorruptImageErrorNotABMPImageFile 107
#define MGK_CorruptImageErrorNotACUTImageFile 108
#define MGK_CorruptImageErrorNotADCMImageFile 109
#define MGK_CorruptImageErrorNotADCXImageFile 110
#define MGK_CorruptImageErrorNotADIBImageFile 111
#define MGK_CorruptImageErrorNotADPXImageFile 112
#define MGK_CorruptImageErrorNotAGIFImageFile 113
#define MGK_CorruptImageErrorNotAJNGImageFile 114
#define MGK_CorruptImageErrorNotAMATLABImageFile 115
#define MGK_CorruptImageErrorNotAMNGImageFile 116
#define MGK_CorruptImageErrorNotAMTVImageFile 117
#define MGK_CorruptImageErrorNotAnARTImageFile 118
#define MGK_CorruptImageErrorNotAnAVIImageFile 119
#define MGK_CorruptImageErrorNotAnAVSImageFile 120
#define MGK_CorruptImageErrorNotAnEMFImageFile 121
#define MGK_CorruptImageErrorNotAnICOImageFile 122
#define MGK_CorruptImageErrorNotAOTBImageFile 123
#define MGK_CorruptImageErrorNotAPCDImageFile 124
#define MGK_CorruptImageErrorNotAPCXImageFile 125
#define MGK_CorruptImageErrorNotAPDBImageFile 126
#define MGK_CorruptImageErrorNotAPICTImageFile 127
#define MGK_CorruptImageErrorNotAPIXImageFile 128
#define MGK_CorruptImageErrorNotAPNGImageFile 129
#define MGK_CorruptImageErrorNotAPNMImageFile 130
#define MGK_CorruptImageErrorNotAPSDImageFile 131
#define MGK_CorruptImageErrorNotAPWPImageFile 132
#define MGK_CorruptImageErrorNotARLEImageFile 133
#define MGK_CorruptImageErrorNotASCTImageFile 134
#define MGK_CorruptImageErrorNotASFWImageFile 135
#define MGK_CorruptImageErrorNotASGIImageFile 136
#define MGK_CorruptImageErrorNotASUNImageFile 137
#define MGK_CorruptImageErrorNotATGAImageFile 138
#define MGK_CorruptImageErrorNotATIMImageFile 139
#define MGK_CorruptImageErrorNotATOPOLImageFile 140
#define MGK_CorruptImageErrorNotAVIFFImageFile 141
#define MGK_CorruptImageErrorNotAWBMPImageFile 142
#define MGK_CorruptImageErrorNotAWPGImageFile 143
#define MGK_CorruptImageErrorNotAXBMImageFile 144
#define MGK_CorruptImageErrorNotAXCFImageFile 145
#define MGK_CorruptImageErrorNotAXPMImageFile 146
#define MGK_CorruptImageErrorNotEnoughTiles 147
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 148
#define MGK_CorruptImageErrorTooMuchImageDataInFile 149
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 150
#define MGK_CorruptImageErrorUnableToReadColorProfile 151
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 152
#define MGK_CorruptImageErrorUnableToReadGenericProfile 153
#define MGK_CorruptImageErrorUnableToReadImageData 154
#define MGK_CorruptImageErrorUnableToReadImageHeader 155
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 156
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 157
#define MGK_CorruptImageErrorUnableToReadSubImageData 158
#define MGK_CorruptImageErrorUnableToReadVIDImage 159
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 160
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 161
#define MGK_CorruptImageErrorUnableToUncompressImage 162
#define MGK_CorruptImageErrorUnexpectedEndOfFile 163
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 164
#define MGK_CorruptImageErrorUnknownPatternType 165
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 166
#define MGK_CorruptImageErrorUnrecognizedImageCompression 167
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 168
#define MGK_CorruptImageFatalErrorUnableToPersistKey 169
#define MGK_CorruptImageWarningCompressionNotValid 170
#define MGK_CorruptImageWarningImproperImageHeader 171
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 172
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 173
#define MGK_CorruptImageWarningSkipToSyncByte 174
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 175
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 176
#define MGK_CorruptImageWarningUnrecognizedImageCompression 177
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 178
#define MGK_DelegateErrorDelegateFailed 179
#define MGK_DelegateErrorFailedToComputeOutputSize 180
#define MGK_DelegateErrorFailedToRenderFile 181
#define MGK_DelegateErrorFailedToScanFile 182
#define MGK_DelegateErrorNoTagFound 183
#define MGK_DelegateErrorPostscriptDelegateFailed 184
#define MGK_DelegateErrorUnableToCreateImage 185
#define MGK_DelegateErrorUnableToCreateImageComponent 186
#define MGK_DelegateErrorUnableToDecodeImageFile 187
#define MGK_DelegateErrorUnableToEncodeImageFile 188
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 189
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 190
#define MGK_DelegateErrorUnableToManageJP2Stream 191
#define MGK_DelegateErrorUnableToWriteSVGFormat 192
#define MGK_DelegateFatalErrorDefault 193
#define MGK_DelegateWarningDefault 194
#define MGK_DrawErrorAlreadyPushingPatternDefinition 195
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 196
#define MGK_DrawErrorUnableToPrint 197
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 198
#define MGK_DrawFatalErrorDefault 199
#define MGK_DrawWarningNotARelativeURL 200
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 201
#define MGK_DrawWarningURLNotFound 202
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 203
#define MGK_FileOpenErrorUnableToOpenFile 204
#define MGK_FileOpenErrorUnableToWriteFile 205
#define MGK_FileOpenFatalErrorDefault 206
#define MGK_FileOpenWarningDefault 207
#define MGK_ImageErrorAngleIsDiscontinuous 208
#define MGK_ImageErrorColorspaceColorProfileMismatch 209
#define MGK_ImageErrorImageColorspaceDiffers 210
#define MGK_ImageErrorImageOpacityDiffers 211
#define MGK_ImageErrorImageSequenceIsRequired 212
#define MGK_ImageErrorImageSizeDiffers 213
#define MGK_ImageErrorInvalidColormapIndex 214
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 215
#define MGK_ImageErrorNoImagesWereFound 216
#define MGK_ImageErrorNoImagesWereLoaded 217
#define MGK_ImageErrorNoLocaleImageAttribute 218
#define MGK_ImageErrorTooManyClusters 219
#define MGK_ImageErrorUnableToAppendImage 220
#define MGK_ImageErrorUnableToAssignProfile 221
#define MGK_ImageErrorUnableToAverageImage 222
#define MGK_ImageErrorUnableToCoalesceImage 223
#define MGK_ImageErrorUnableToCompareImages 224
#define MGK_ImageErrorUnableToCreateImageMosaic 225
#define MGK_ImageErrorUnableToCreateStereoImage 226
#define MGK_ImageErrorUnableToDeconstructImageSequence 227
#define MGK_ImageErrorUnableToFlattenImage 228
#define MGK_ImageErrorUnableToResizeImage 229
#define MGK_ImageErrorUnableToSegmentImage 230
#define MGK_ImageErrorUnableToSetClipMask 231
#define MGK_ImageErrorUnableToShearImage 232
#define MGK_ImageErrorWidthOrHeightExceedsLimit 233
#define MGK_ImageFatalErrorUnableToPersistKey 234
#define MGK_ImageWarningDefault 235
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 236
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 237
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 238
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 239
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 240
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 241
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 242
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 243
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 244
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 245
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 246
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 247
#define MGK_MissingDelegateFatalErrorDefault 248
#define MGK_MissingDelegateWarningDefault 249
#define MGK_ModuleErrorFailedToCloseModule 250
#define MGK_ModuleErrorFailedToFindSymbol 251
#define MGK_ModuleErrorUnableToLoadModule 252
#define MGK_ModuleErrorUnableToRegisterImageFormat 253
#define MGK_ModuleErrorUnrecognizedModule 254
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 255
#define MGK_ModuleWarningDefault 256
#define MGK_OptionErrorBevelWidthIsNegative 257
#define MGK_OptionErrorColorSeparatedImageRequired 258
#define MGK_OptionErrorFrameIsLessThanImageSize 259
#define MGK_OptionErrorGeometryDimensionsAreZero 260
#define MGK_OptionErrorGeometryDoesNotContainImage 261
#define MGK_OptionErrorImagesAreNotTheSameSize 262
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 263
#define MGK_OptionErrorImageSmallerThanKernelWidth 264
#define MGK_OptionErrorImageSmallerThanRadius 265
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 266
#define MGK_OptionErrorInputImagesAlreadySpecified 267
#define MGK_OptionErrorKernelRadiusIsTooSmall 268
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 269
#define MGK_OptionErrorMissingAffineMatrix 270
#define MGK_OptionErrorMissingAmount 271
#define MGK_OptionErrorMissingAmplitude 272
#define MGK_OptionErrorMissingAnImageFilename 273
#define MGK_OptionErrorMissingArgument 274
#define MGK_OptionErrorMissingAzimuth 275
#define MGK_OptionErrorMissingBackgroundColor 276
#define MGK_OptionErrorMissingBevelWidth 277
#define MGK_OptionErrorMissingBorderColor 278
#define MGK_OptionErrorMissingClippingPathName 279
#define MGK_OptionErrorMissingCoefficients 280
#define MGK_OptionErrorMissingColorizeValue 281
#define MGK_OptionErrorMissingColors 282
#define MGK_OptionErrorMissingComment 283
#define MGK_OptionErrorMissingCompressionQuality 284
#define MGK_OptionErrorMissingCycleAmount 285
#define MGK_OptionErrorMissingDelay 286
#define MGK_OptionErrorMissingDisposeMethod 287
#define MGK_OptionErrorMissingDissolveValue 288
#define MGK_OptionErrorMissingDistance 289
#define MGK_OptionErrorMissingDrawingPrimitive 290
#define MGK_OptionErrorMissingEnodingType 291
#define MGK_OptionErrorMissingEventMask 292
#define MGK_OptionErrorMissingFilename 293
#define MGK_OptionErrorMissingFillColor 294
#define MGK_OptionErrorMissingFontName 295
#define MGK_OptionErrorMissingFormatSpecifier 296
#define MGK_OptionErrorMissingGeometry 297
#define MGK_OptionErrorMissingImageDepth 298
#define MGK_OptionErrorMissingImageFrames 299
#define MGK_OptionErrorMissingImageGamma 300
#define MGK_OptionErrorMissingImageIterations 301
#define MGK_OptionErrorMissingImageLabel 302
#define MGK_OptionErrorMissingImageProfile 303
#define MGK_OptionErrorMissingImageRotation 304
#define MGK_OptionErrorMissingImageScene 305
#define MGK_OptionErrorMissingImageType 306
#define MGK_OptionErrorMissingImplodeAmount 307
#define MGK_OptionErrorMissingListName 308
#define MGK_OptionErrorMissingLogFormat 309
#define MGK_OptionErrorMissingMatrix 310
#define MGK_OptionErrorMissingMatteColor 311
#define MGK_OptionErrorMissingOffset 312
#define MGK_OptionErrorMissingOpaqueColor 313
#define MGK_OptionErrorMissingOrder 314
#define MGK_OptionErrorMissingPageGeometry 315
#define MGK_OptionErrorMissingPaintRadius 316
#define MGK_OptionErrorMissingPassword 317
#define MGK_OptionErrorMissingPoint 318
#define MGK_OptionErrorMissingPointsize 319
#define MGK_OptionErrorMissingResourceLimit 320
#define MGK_OptionErrorMissingResourceType 321
#define MGK_OptionErrorMissingSeedValue 322
#define MGK_OptionErrorMissingServerName 323
#define MGK_OptionErrorMissingSpreadAmount 324
#define MGK_OptionErrorMissingThreshold 325
#define MGK_OptionErrorMissingTile 326
#define MGK_OptionErrorMissingTitle 327
#define MGK_OptionErrorMissingTransparentColor 328
#define MGK_OptionErrorMissingTreeDepth 329
#define MGK_OptionErrorMissingType 330
#define MGK_OptionErrorMissingViewTransform 331
#define MGK_OptionErrorMissingVirtualPixelMethod 332
#define MGK_OptionErrorMissingWidth 333
#define MGK_OptionErrorMustSpecifyAnImageName 334
#define MGK_OptionErrorMustSpecifyImageSize 335
#define MGK_OptionErrorNoBlobDefined 336
#define MGK_OptionErrorNoImagesDefined 337
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 338
#define MGK_OptionErrorNoProfileNameWasGiven 339
#define MGK_OptionErrorNullBlobArgument 340
#define MGK_OptionErrorReferenceImageRequired 341
#define MGK_OptionErrorReferenceIsNotMyType 342
#define MGK_OptionErrorSteganoImageRequired 343
#define MGK_OptionErrorStereoImageRequired 344
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 345
#define MGK_OptionErrorUnableToAddOrRemoveProfile 346
#define MGK_OptionErrorUnableToAllocateICCProfile 347
#define MGK_OptionErrorUnableToAverageImageSequence 348
#define MGK_OptionErrorUnableToBlurImage 349
#define MGK_OptionErrorUnableToChopImage 350
#define MGK_OptionErrorUnableToConstituteImage 351
#define MGK_OptionErrorUnableToConvolveImage 352
#define MGK_OptionErrorUnableToEdgeImage 353
#define MGK_OptionErrorUnableToEqualizeImage 354
#define MGK_OptionErrorUnableToFilterImage 355
#define MGK_OptionErrorUnableToFormatImageMetadata 356
#define MGK_OptionErrorUnableToFrameImage 357
#define MGK_OptionErrorUnableToOilPaintImage 358
#define MGK_OptionErrorUnableToPaintImage 359
#define MGK_OptionErrorUnableToRaiseImage 360
#define MGK_OptionErrorUnableToSharpenImage 361
#define MGK_OptionErrorUnableToThresholdImage 362
#define MGK_OptionErrorUnableToWaveImage 363
#define MGK_OptionErrorUnrecognizedAttribute 364
#define MGK_OptionErrorUnrecognizedChannelType 365
#define MGK_OptionErrorUnrecognizedColor 366
#define MGK_OptionErrorUnrecognizedColormapType 367
#define MGK_OptionErrorUnrecognizedColorspace 368
#define MGK_OptionErrorUnrecognizedComposeOperator 369
#define MGK_OptionErrorUnrecognizedDisposeMethod 370
#define MGK_OptionErrorUnrecognizedElement 371
#define MGK_OptionErrorUnrecognizedEndianType 372
#define MGK_OptionErrorUnrecognizedGravityType 373
#define MGK_OptionErrorUnrecognizedImageCompression 374
#define MGK_OptionErrorUnrecognizedImageFilter 375
#define MGK_OptionErrorUnrecognizedImageFormat 376
#define MGK_OptionErrorUnrecognizedImageMode 377
#define MGK_OptionErrorUnrecognizedImageType 378
#define MGK_OptionErrorUnrecognizedIntentType 379
#define MGK_OptionErrorUnrecognizedInterlaceType 380
#define MGK_OptionErrorUnrecognizedListType 381
#define MGK_OptionErrorUnrecognizedModeType 382
#define MGK_OptionErrorUnrecognizedNoiseType 383
#define MGK_OptionErrorUnrecognizedOption 384
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 385
#define MGK_OptionErrorUnrecognizedPixelMap 386
#define MGK_OptionErrorUnrecognizedPreviewType 387
#define MGK_OptionErrorUnrecognizedResourceType 388
#define MGK_OptionErrorUnrecognizedType 389
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 390
#define MGK_OptionErrorUsageError 391
#define MGK_OptionFatalErrorDeprecatedOption 392
#define MGK_OptionFatalErrorInvalidColorspaceType 393
#define MGK_OptionFatalErrorInvalidEndianType 394
#define MGK_OptionFatalErrorInvalidImageType 395
#define MGK_OptionFatalErrorInvalidInterlaceType 396
#define MGK_OptionFatalErrorMissingAnImageFilename 397
#define MGK_OptionFatalErrorMissingBevelWidth 398
#define MGK_OptionFatalErrorMissingBorderColor 399
#define MGK_OptionFatalErrorMissingColor 400
#define MGK_OptionFatalErrorMissingColors 401
#define MGK_OptionFatalErrorMissingCommand 402
#define MGK_OptionFatalErrorMissingComment 403
#define MGK_OptionFatalErrorMissingCompressionQuality 404
#define MGK_OptionFatalErrorMissingDegrees 405
#define MGK_OptionFatalErrorMissingDepth 406
#define MGK_OptionFatalErrorMissingDisposeMethod 407
#define MGK_OptionFatalErrorMissingEncodingType 408
#define MGK_OptionFatalErrorMissingEventMask 409
#define MGK_OptionFatalErrorMissingFactor 410
#define MGK_OptionFatalErrorMissingFilename 411
#define MGK_OptionFatalErrorMissingFontName 412
#define MGK_OptionFatalErrorMissingForeground 413
#define MGK_OptionFatalErrorMissingGeometry 414
#define MGK_OptionFatalErrorMissingIDNameOrRoot 415
#define MGK_OptionFatalErrorMissingImageDepth 416
#define MGK_OptionFatalErrorMissingLabelName 417
#define MGK_OptionFatalErrorMissingLevel 418
#define MGK_OptionFatalErrorMissingLogFormat 419
#define MGK_OptionFatalErrorMissingMapType 420
#define MGK_OptionFatalErrorMissingMethod 421
#define MGK_OptionFatalErrorMissingName 422
#define MGK_OptionFatalErrorMissingPageGeometry 423
#define MGK_OptionFatalErrorMissingPassword 424
#define MGK_OptionFatalErrorMissingQuality 425
#define MGK_OptionFatalErrorMissingResourceLimit 426
#define MGK_OptionFatalErrorMissingResourceType 427
#define MGK_OptionFatalErrorMissingSceneNumber 428
#define MGK_OptionFatalErrorMissingSeconds 429
#define MGK_OptionFatalErrorMissingServerName 430
#define MGK_OptionFatalErrorMissingSize 431
#define MGK_OptionFatalErrorMissingSnaps 432
#define MGK_OptionFatalErrorMissingThreshold 433
#define MGK_OptionFatalErrorMissingTitle 434
#define MGK_OptionFatalErrorMissingType 435
#define MGK_OptionFatalErrorMissingValue 436
#define MGK_OptionFatalErrorMissingVirtualPixelMethod 437
#define MGK_OptionFatalErrorMissingVisualClass 438
#define MGK_OptionFatalErrorMissingWidth 439
#define MGK_OptionFatalErrorNoImagesWereLoaded 440
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 441
#define MGK_OptionFatalErrorUnableToOpenXServer 442
#define MGK_OptionFatalErrorUnableToPersistKey 443
#define MGK_OptionFatalErrorUnrecognizedColormapType 444
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 445
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 446
#define MGK_OptionFatalErrorUnrecognizedEndianType 447
#define MGK_OptionFatalErrorUnrecognizedFilterType 448
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 449
#define MGK_OptionFatalErrorUnrecognizedImageType 450
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 451
#define MGK_OptionFatalErrorUnrecognizedOption 452
#define MGK_OptionFatalErrorUnrecognizedResourceType 453
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 454
#define MGK_OptionWarningUnrecognizedColor 455
#define MGK_RegistryErrorImageExpected 456
#define MGK_RegistryErrorImageInfoExpected 457
#define MGK_RegistryErrorStructureSizeMismatch 458
#define MGK_RegistryErrorUnableToGetRegistryID 459
#define MGK_RegistryErrorUnableToLocateImage 460
#define MGK_RegistryErrorUnableToSetRegistry 461
#define MGK_RegistryFatalErrorDefault 462
#define MGK_RegistryWarningDefault 463
#define MGK_ResourceLimitErrorCacheResourcesExhausted 464
#define MGK_ResourceLimitErrorMemoryAllocationFailed 465
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 466
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 467
#define MGK_ResourceLimitErrorUnableToAddColorProfile 468
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 469
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 470
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 471
#define MGK_ResourceLimitErrorUnableToAllocateColormap 472
#define MGK_ResourceLimitErrorUnableToAllocateString 473
#define MGK_ResourceLimitErrorUnableToAnnotateImage 474
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 475
#define MGK_ResourceLimitErrorUnableToCloneImage 476
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 477
#define MGK_ResourceLimitErrorUnableToConstituteImage 478
#define MGK_ResourceLimitErrorUnableToConvertFont 479
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 480
#define MGK_ResourceLimitErrorUnableToCreateColormap 481
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 482
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 483
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 484
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 485
#define MGK_ResourceLimitErrorUnableToCreateXWindow 486
#define MGK_ResourceLimitErrorUnableToCropImage 487
#define MGK_ResourceLimitErrorUnableToDespeckleImage 488
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 489
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 490
#define MGK_ResourceLimitErrorUnableToDitherImage 491
#define MGK_ResourceLimitErrorUnableToDrawOnImage 492
#define MGK_ResourceLimitErrorUnableToEdgeImage 493
#define MGK_ResourceLimitErrorUnableToEmbossImage 494
#define MGK_ResourceLimitErrorUnableToEnhanceImage 495
#define MGK_ResourceLimitErrorUnableToFloodfillImage 496
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 497
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 498
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 499
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 500
#define MGK_ResourceLimitErrorUnableToLevelImage 501
#define MGK_ResourceLimitErrorUnableToMagnifyImage 502
#define MGK_ResourceLimitErrorUnableToManageColor 503
#define MGK_ResourceLimitErrorUnableToMapImage 504
#define MGK_ResourceLimitErrorUnableToMapImageSequence 505
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 506
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 507
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 508
#define MGK_ResourceLimitErrorUnableToNormalizeImage 509
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 510
#define MGK_ResourceLimitErrorUnableToQuantizeImage 511
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 512
#define MGK_ResourceLimitErrorUnableToReadTextChunk 513
#define MGK_ResourceLimitErrorUnableToReadXImage 514
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 515
#define MGK_ResourceLimitErrorUnableToResizeImage 516
#define MGK_ResourceLimitErrorUnableToRotateImage 517
#define MGK_ResourceLimitErrorUnableToSampleImage 518
#define MGK_ResourceLimitErrorUnableToScaleImage 519
#define MGK_ResourceLimitErrorUnableToSelectImage 520
#define MGK_ResourceLimitErrorUnableToSharpenImage 521
#define MGK_ResourceLimitErrorUnableToShaveImage 522
#define MGK_ResourceLimitErrorUnableToShearImage 523
#define MGK_ResourceLimitErrorUnableToSortImageColormap 524
#define MGK_ResourceLimitErrorUnableToThresholdImage 525
#define MGK_ResourceLimitErrorUnableToTransformColorspace 526
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 527
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 528
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 529
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 530
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 531
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 532
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 533
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 534
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 535
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 536
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 537
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 538
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 539
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 540
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 541
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 542
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 543
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 544
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 545
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 546
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 547
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 548
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 549
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 550
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 551
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 552
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 553
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 554
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 555
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 556
#define MGK_ResourceLimitFatalErrorUnableToConvertText 557
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 558
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 559
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 560
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 561
#define MGK_ResourceLimitWarningMemoryAllocationFailed 562
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 563
#define MGK_StreamErrorNoStreamHandlerIsDefined 564
#define MGK_StreamErrorPixelCacheIsNotOpen 565
#define MGK_StreamErrorUnableToAcquirePixelStream 566
#define MGK_StreamErrorUnableToSetPixelStream 567
#define MGK_StreamErrorUnableToSyncPixelStream 568
#define MGK_StreamFatalErrorDefault 569
#define MGK_StreamWarningDefault 570
#define MGK_TypeErrorFontSubstitutionRequired 571
#define MGK_TypeErrorUnableToGetTypeMetrics 572
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 573
#define MGK_TypeErrorUnableToReadFont 574
#define MGK_TypeErrorUnrecognizedFontEncoding 575
#define MGK_TypeFatalErrorDefault 576
#define MGK_TypeWarningDefault 577
#define MGK_XServerErrorColorIsNotKnownToServer 578
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 579
#define MGK_XServerErrorStandardColormapIsNotInitialized 580
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 581
#define MGK_XServerErrorUnableToCreateBitmap 582
#define MGK_XServerErrorUnableToCreateColormap 583
#define MGK_XServerErrorUnableToCreatePixmap 584
#define MGK_XServerErrorUnableToCreateProperty 585
#define MGK_XServerErrorUnableToCreateStandardColormap 586
#define MGK_XServerErrorUnableToDisplayImageInfo 587
#define MGK_XServerErrorUnableToGetProperty 588
#define MGK_XServerErrorUnableToGetStandardColormap 589
#define MGK_XServerErrorUnableToGetVisual 590
#define MGK_XServerErrorUnableToGrabMouse 591
#define MGK_XServerErrorUnableToLoadFont 592
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 593
#define MGK_XServerErrorUnableToOpenXServer 594
#define MGK_XServerErrorUnableToReadXAttributes 595
#define MGK_XServerErrorUnableToReadXWindowImage 596
#define MGK_XServerErrorUnrecognizedColormapType 597
#define MGK_XServerErrorUnrecognizedGravityType 598
#define MGK_XServerErrorUnrecognizedVisualSpecifier 599
#define MGK_XServerFatalErrorUnableToAllocateXHints 600
#define MGK_XServerFatalErrorUnableToCreateCursor 601
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 602
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 603
#define MGK_XServerFatalErrorUnableToCreateTextProperty 604
#define MGK_XServerFatalErrorUnableToCreateXImage 605
#define MGK_XServerFatalErrorUnableToCreateXPixmap 606
#define MGK_XServerFatalErrorUnableToCreateXWindow 607
#define MGK_XServerFatalErrorUnableToDisplayImage 608
#define MGK_XServerFatalErrorUnableToDitherImage 609
#define MGK_XServerFatalErrorUnableToGetPixelInfo 610
#define MGK_XServerFatalErrorUnableToGetVisual 611
#define MGK_XServerFatalErrorUnableToLoadFont 612
#define MGK_XServerFatalErrorUnableToMakeXWindow 613
#define MGK_XServerFatalErrorUnableToOpenXServer 614
#define MGK_XServerFatalErrorUnableToViewFonts 615
#define MGK_XServerWarningUnableToGetVisual 616
#define MGK_XServerWarningUsingDefaultVisual 617

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
    { "Image/FatalError", 233, ImageFatalError },
    { "Image/Warning", 234, ImageWarning },
    { "Missing/Delegate/Error", 235, MissingDelegateError },
    { "Missing/Delegate/FatalError", 247, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 248, MissingDelegateWarning },
    { "Module/Error", 249, ModuleError },
    { "Module/FatalError", 254, ModuleFatalError },
    { "Module/Warning", 255, ModuleWarning },
    { "Option/Error", 256, OptionError },
    { "Option/FatalError", 391, OptionFatalError },
    { "Option/Warning", 454, OptionWarning },
    { "Registry/Error", 455, RegistryError },
    { "Registry/FatalError", 461, RegistryFatalError },
    { "Registry/Warning", 462, RegistryWarning },
    { "Resource/Limit/Error", 463, ResourceLimitError },
    { "Resource/Limit/FatalError", 526, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 561, ResourceLimitWarning },
    { "Stream/Error", 562, StreamError },
    { "Stream/FatalError", 568, StreamFatalError },
    { "Stream/Warning", 569, StreamWarning },
    { "Type/Error", 570, TypeError },
    { "Type/FatalError", 575, TypeFatalError },
    { "Type/Warning", 576, TypeWarning },
    { "XServer/Error", 577, XServerError },
    { "XServer/FatalError", 599, XServerFatalError },
    { "XServer/Warning", 615, XServerWarning },
    { 0, 617, 0 }
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
    { "ColorspaceColorProfileMismatch", 209 },
    { "ImageColorspaceDiffers", 210 },
    { "ImageOpacityDiffers", 211 },
    { "ImageSequenceIsRequired", 212 },
    { "ImageSizeDiffers", 213 },
    { "InvalidColormapIndex", 214 },
    { "LeftAndRightImageSizesDiffer", 215 },
    { "NoImagesWereFound", 216 },
    { "NoImagesWereLoaded", 217 },
    { "NoLocaleImageAttribute", 218 },
    { "TooManyClusters", 219 },
    { "UnableToAppendImage", 220 },
    { "UnableToAssignProfile", 221 },
    { "UnableToAverageImage", 222 },
    { "UnableToCoalesceImage", 223 },
    { "UnableToCompareImages", 224 },
    { "UnableToCreateImageMosaic", 225 },
    { "UnableToCreateStereoImage", 226 },
    { "UnableToDeconstructImageSequence", 227 },
    { "UnableToFlattenImage", 228 },
    { "UnableToResizeImage", 229 },
    { "UnableToSegmentImage", 230 },
    { "UnableToSetClipMask", 231 },
    { "UnableToShearImage", 232 },
    { "WidthOrHeightExceedsLimit", 233 },
    { "UnableToPersistKey", 234 },
    { "Default", 235 },
    { "DPSLibraryIsNotAvailable", 236 },
    { "FPXLibraryIsNotAvailable", 237 },
    { "FreeTypeLibraryIsNotAvailable", 238 },
    { "JPEGLibraryIsNotAvailable", 239 },
    { "LCMSLibraryIsNotAvailable", 240 },
    { "LZWEncodingNotEnabled", 241 },
    { "NoDecodeDelegateForThisImageFormat", 242 },
    { "NoEncodeDelegateForThisImageFormat", 243 },
    { "TIFFLibraryIsNotAvailable", 244 },
    { "XMLLibraryIsNotAvailable", 245 },
    { "XWindowLibraryIsNotAvailable", 246 },
    { "ZipLibraryIsNotAvailable", 247 },
    { "Default", 248 },
    { "Default", 249 },
    { "FailedToCloseModule", 250 },
    { "FailedToFindSymbol", 251 },
    { "UnableToLoadModule", 252 },
    { "UnableToRegisterImageFormat", 253 },
    { "UnrecognizedModule", 254 },
    { "UnableToInitializeModuleLoader", 255 },
    { "Default", 256 },
    { "BevelWidthIsNegative", 257 },
    { "ColorSeparatedImageRequired", 258 },
    { "FrameIsLessThanImageSize", 259 },
    { "GeometryDimensionsAreZero", 260 },
    { "GeometryDoesNotContainImage", 261 },
    { "ImagesAreNotTheSameSize", 262 },
    { "ImageSizeMustExceedBevelWidth", 263 },
    { "ImageSmallerThanKernelWidth", 264 },
    { "ImageSmallerThanRadius", 265 },
    { "ImageWidthsOrHeightsDiffer", 266 },
    { "InputImagesAlreadySpecified", 267 },
    { "KernelRadiusIsTooSmall", 268 },
    { "KernelWidthMustBeAnOddNumber", 269 },
    { "MissingAffineMatrix", 270 },
    { "MissingAmount", 271 },
    { "MissingAmplitude", 272 },
    { "MissingAnImageFilename", 273 },
    { "MissingArgument", 274 },
    { "MissingAzimuth", 275 },
    { "MissingBackgroundColor", 276 },
    { "MissingBevelWidth", 277 },
    { "MissingBorderColor", 278 },
    { "MissingClippingPathName", 279 },
    { "MissingCoefficients", 280 },
    { "MissingColorizeValue", 281 },
    { "MissingColors", 282 },
    { "MissingComment", 283 },
    { "MissingCompressionQuality", 284 },
    { "MissingCycleAmount", 285 },
    { "MissingDelay", 286 },
    { "MissingDisposeMethod", 287 },
    { "MissingDissolveValue", 288 },
    { "MissingDistance", 289 },
    { "MissingDrawingPrimitive", 290 },
    { "MissingEnodingType", 291 },
    { "MissingEventMask", 292 },
    { "MissingFilename", 293 },
    { "MissingFillColor", 294 },
    { "MissingFontName", 295 },
    { "MissingFormatSpecifier", 296 },
    { "MissingGeometry", 297 },
    { "MissingImageDepth", 298 },
    { "MissingImageFrames", 299 },
    { "MissingImageGamma", 300 },
    { "MissingImageIterations", 301 },
    { "MissingImageLabel", 302 },
    { "MissingImageProfile", 303 },
    { "MissingImageRotation", 304 },
    { "MissingImageScene", 305 },
    { "MissingImageType", 306 },
    { "MissingImplodeAmount", 307 },
    { "MissingListName", 308 },
    { "MissingLogFormat", 309 },
    { "MissingMatrix", 310 },
    { "MissingMatteColor", 311 },
    { "MissingOffset", 312 },
    { "MissingOpaqueColor", 313 },
    { "MissingOrder", 314 },
    { "MissingPageGeometry", 315 },
    { "MissingPaintRadius", 316 },
    { "MissingPassword", 317 },
    { "MissingPoint", 318 },
    { "MissingPointsize", 319 },
    { "MissingResourceLimit", 320 },
    { "MissingResourceType", 321 },
    { "MissingSeedValue", 322 },
    { "MissingServerName", 323 },
    { "MissingSpreadAmount", 324 },
    { "MissingThreshold", 325 },
    { "MissingTile", 326 },
    { "MissingTitle", 327 },
    { "MissingTransparentColor", 328 },
    { "MissingTreeDepth", 329 },
    { "MissingType", 330 },
    { "MissingViewTransform", 331 },
    { "MissingVirtualPixelMethod", 332 },
    { "MissingWidth", 333 },
    { "MustSpecifyAnImageName", 334 },
    { "MustSpecifyImageSize", 335 },
    { "NoBlobDefined", 336 },
    { "NoImagesDefined", 337 },
    { "NonzeroWidthAndHeightRequired", 338 },
    { "NoProfileNameWasGiven", 339 },
    { "NullBlobArgument", 340 },
    { "ReferenceImageRequired", 341 },
    { "ReferenceIsNotMyType", 342 },
    { "SteganoImageRequired", 343 },
    { "StereoImageRequired", 344 },
    { "SubimageSpecificationReturnsNoImages", 345 },
    { "UnableToAddOrRemoveProfile", 346 },
    { "UnableToAllocateICCProfile", 347 },
    { "UnableToAverageImageSequence", 348 },
    { "UnableToBlurImage", 349 },
    { "UnableToChopImage", 350 },
    { "UnableToConstituteImage", 351 },
    { "UnableToConvolveImage", 352 },
    { "UnableToEdgeImage", 353 },
    { "UnableToEqualizeImage", 354 },
    { "UnableToFilterImage", 355 },
    { "UnableToFormatImageMetadata", 356 },
    { "UnableToFrameImage", 357 },
    { "UnableToOilPaintImage", 358 },
    { "UnableToPaintImage", 359 },
    { "UnableToRaiseImage", 360 },
    { "UnableToSharpenImage", 361 },
    { "UnableToThresholdImage", 362 },
    { "UnableToWaveImage", 363 },
    { "UnrecognizedAttribute", 364 },
    { "UnrecognizedChannelType", 365 },
    { "UnrecognizedColor", 366 },
    { "UnrecognizedColormapType", 367 },
    { "UnrecognizedColorspace", 368 },
    { "UnrecognizedComposeOperator", 369 },
    { "UnrecognizedDisposeMethod", 370 },
    { "UnrecognizedElement", 371 },
    { "UnrecognizedEndianType", 372 },
    { "UnrecognizedGravityType", 373 },
    { "UnrecognizedImageCompression", 374 },
    { "UnrecognizedImageFilter", 375 },
    { "UnrecognizedImageFormat", 376 },
    { "UnrecognizedImageMode", 377 },
    { "UnrecognizedImageType", 378 },
    { "UnrecognizedIntentType", 379 },
    { "UnrecognizedInterlaceType", 380 },
    { "UnrecognizedListType", 381 },
    { "UnrecognizedModeType", 382 },
    { "UnrecognizedNoiseType", 383 },
    { "UnrecognizedOption", 384 },
    { "UnrecognizedPerlMagickMethod", 385 },
    { "UnrecognizedPixelMap", 386 },
    { "UnrecognizedPreviewType", 387 },
    { "UnrecognizedResourceType", 388 },
    { "UnrecognizedType", 389 },
    { "UnrecognizedVirtualPixelMethod", 390 },
    { "UsageError", 391 },
    { "DeprecatedOption", 392 },
    { "InvalidColorspaceType", 393 },
    { "InvalidEndianType", 394 },
    { "InvalidImageType", 395 },
    { "InvalidInterlaceType", 396 },
    { "MissingAnImageFilename", 397 },
    { "MissingBevelWidth", 398 },
    { "MissingBorderColor", 399 },
    { "MissingColor", 400 },
    { "MissingColors", 401 },
    { "MissingCommand", 402 },
    { "MissingComment", 403 },
    { "MissingCompressionQuality", 404 },
    { "MissingDegrees", 405 },
    { "MissingDepth", 406 },
    { "MissingDisposeMethod", 407 },
    { "MissingEncodingType", 408 },
    { "MissingEventMask", 409 },
    { "MissingFactor", 410 },
    { "MissingFilename", 411 },
    { "MissingFontName", 412 },
    { "MissingForeground", 413 },
    { "MissingGeometry", 414 },
    { "MissingIDNameOrRoot", 415 },
    { "MissingImageDepth", 416 },
    { "MissingLabelName", 417 },
    { "MissingLevel", 418 },
    { "MissingLogFormat", 419 },
    { "MissingMapType", 420 },
    { "MissingMethod", 421 },
    { "MissingName", 422 },
    { "MissingPageGeometry", 423 },
    { "MissingPassword", 424 },
    { "MissingQuality", 425 },
    { "MissingResourceLimit", 426 },
    { "MissingResourceType", 427 },
    { "MissingSceneNumber", 428 },
    { "MissingSeconds", 429 },
    { "MissingServerName", 430 },
    { "MissingSize", 431 },
    { "MissingSnaps", 432 },
    { "MissingThreshold", 433 },
    { "MissingTitle", 434 },
    { "MissingType", 435 },
    { "MissingValue", 436 },
    { "MissingVirtualPixelMethod", 437 },
    { "MissingVisualClass", 438 },
    { "MissingWidth", 439 },
    { "NoImagesWereLoaded", 440 },
    { "OptionLengthExceedsLimit", 441 },
    { "UnableToOpenXServer", 442 },
    { "UnableToPersistKey", 443 },
    { "UnrecognizedColormapType", 444 },
    { "UnrecognizedColorspaceType", 445 },
    { "UnrecognizedDisposeMethod", 446 },
    { "UnrecognizedEndianType", 447 },
    { "UnrecognizedFilterType", 448 },
    { "UnrecognizedImageCompressionType", 449 },
    { "UnrecognizedImageType", 450 },
    { "UnrecognizedInterlaceType", 451 },
    { "UnrecognizedOption", 452 },
    { "UnrecognizedResourceType", 453 },
    { "UnrecognizedVirtualPixelMethod", 454 },
    { "UnrecognizedColor", 455 },
    { "ImageExpected", 456 },
    { "ImageInfoExpected", 457 },
    { "StructureSizeMismatch", 458 },
    { "UnableToGetRegistryID", 459 },
    { "UnableToLocateImage", 460 },
    { "UnableToSetRegistry", 461 },
    { "Default", 462 },
    { "Default", 463 },
    { "CacheResourcesExhausted", 464 },
    { "MemoryAllocationFailed", 465 },
    { "NoPixelsDefinedInCache", 466 },
    { "PixelCacheAllocationFailed", 467 },
    { "UnableToAddColorProfile", 468 },
    { "UnableToAddGenericProfile", 469 },
    { "UnableToAddIPTCProfile", 470 },
    { "UnableToAllocateCoefficients", 471 },
    { "UnableToAllocateColormap", 472 },
    { "UnableToAllocateString", 473 },
    { "UnableToAnnotateImage", 474 },
    { "UnableToAverageImageSequence", 475 },
    { "UnableToCloneImage", 476 },
    { "UnableToComputeImageSignature", 477 },
    { "UnableToConstituteImage", 478 },
    { "UnableToConvertFont", 479 },
    { "UnableToConvertStringToTokens", 480 },
    { "UnableToCreateColormap", 481 },
    { "UnableToCreateColorTransform", 482 },
    { "UnableToCreateCommandWidget", 483 },
    { "UnableToCreateImageGroup", 484 },
    { "UnableToCreateImageMontage", 485 },
    { "UnableToCreateXWindow", 486 },
    { "UnableToCropImage", 487 },
    { "UnableToDespeckleImage", 488 },
    { "UnableToDetermineImageClass", 489 },
    { "UnableToDetermineTheNumberOfImageColors", 490 },
    { "UnableToDitherImage", 491 },
    { "UnableToDrawOnImage", 492 },
    { "UnableToEdgeImage", 493 },
    { "UnableToEmbossImage", 494 },
    { "UnableToEnhanceImage", 495 },
    { "UnableToFloodfillImage", 496 },
    { "UnableToGammaCorrectImage", 497 },
    { "UnableToGetBestIconSize", 498 },
    { "UnableToGetFromRegistry", 499 },
    { "UnableToGetPackageInfo", 500 },
    { "UnableToLevelImage", 501 },
    { "UnableToMagnifyImage", 502 },
    { "UnableToManageColor", 503 },
    { "UnableToMapImage", 504 },
    { "UnableToMapImageSequence", 505 },
    { "UnableToMedianFilterImage", 506 },
    { "UnableToMotionBlurImage", 507 },
    { "UnableToNoiseFilterImage", 508 },
    { "UnableToNormalizeImage", 509 },
    { "UnableToOpenColorProfile", 510 },
    { "UnableToQuantizeImage", 511 },
    { "UnableToQuantizeImageSequence", 512 },
    { "UnableToReadTextChunk", 513 },
    { "UnableToReadXImage", 514 },
    { "UnableToReadXServerColormap", 515 },
    { "UnableToResizeImage", 516 },
    { "UnableToRotateImage", 517 },
    { "UnableToSampleImage", 518 },
    { "UnableToScaleImage", 519 },
    { "UnableToSelectImage", 520 },
    { "UnableToSharpenImage", 521 },
    { "UnableToShaveImage", 522 },
    { "UnableToShearImage", 523 },
    { "UnableToSortImageColormap", 524 },
    { "UnableToThresholdImage", 525 },
    { "UnableToTransformColorspace", 526 },
    { "MemoryAllocationFailed", 527 },
    { "UnableToAllocateAscii85Info", 528 },
    { "UnableToAllocateCacheInfo", 529 },
    { "UnableToAllocateCacheView", 530 },
    { "UnableToAllocateColorInfo", 531 },
    { "UnableToAllocateDashPattern", 532 },
    { "UnableToAllocateDelegateInfo", 533 },
    { "UnableToAllocateDerivatives", 534 },
    { "UnableToAllocateDrawContext", 535 },
    { "UnableToAllocateDrawInfo", 536 },
    { "UnableToAllocateDrawingWand", 537 },
    { "UnableToAllocateGammaMap", 538 },
    { "UnableToAllocateImage", 539 },
    { "UnableToAllocateImagePixels", 540 },
    { "UnableToAllocateLogInfo", 541 },
    { "UnableToAllocateMagicInfo", 542 },
    { "UnableToAllocateMagickInfo", 543 },
    { "UnableToAllocateModuleInfo", 544 },
    { "UnableToAllocateMontageInfo", 545 },
    { "UnableToAllocateQuantizeInfo", 546 },
    { "UnableToAllocateRegistryInfo", 547 },
    { "UnableToAllocateSemaphoreInfo", 548 },
    { "UnableToAllocateString", 549 },
    { "UnableToAllocateTypeInfo", 550 },
    { "UnableToAnimateImageSequence", 551 },
    { "UnableToCloneBlobInfo", 552 },
    { "UnableToCloneCacheInfo", 553 },
    { "UnableToCloneImage", 554 },
    { "UnableToCloneImageInfo", 555 },
    { "UnableToConcatenateString", 556 },
    { "UnableToConvertText", 557 },
    { "UnableToCreateColormap", 558 },
    { "UnableToDisplayImage", 559 },
    { "UnableToEscapeString", 560 },
    { "UnableToInterpretMSLImage", 561 },
    { "MemoryAllocationFailed", 562 },
    { "ImageDoesNotContainTheStreamGeometry", 563 },
    { "NoStreamHandlerIsDefined", 564 },
    { "PixelCacheIsNotOpen", 565 },
    { "UnableToAcquirePixelStream", 566 },
    { "UnableToSetPixelStream", 567 },
    { "UnableToSyncPixelStream", 568 },
    { "Default", 569 },
    { "Default", 570 },
    { "FontSubstitutionRequired", 571 },
    { "UnableToGetTypeMetrics", 572 },
    { "UnableToInitializeFreetypeLibrary", 573 },
    { "UnableToReadFont", 574 },
    { "UnrecognizedFontEncoding", 575 },
    { "Default", 576 },
    { "Default", 577 },
    { "ColorIsNotKnownToServer", 578 },
    { "NoWindowWithSpecifiedIDExists", 579 },
    { "StandardColormapIsNotInitialized", 580 },
    { "UnableToConnectToRemoteDisplay", 581 },
    { "UnableToCreateBitmap", 582 },
    { "UnableToCreateColormap", 583 },
    { "UnableToCreatePixmap", 584 },
    { "UnableToCreateProperty", 585 },
    { "UnableToCreateStandardColormap", 586 },
    { "UnableToDisplayImageInfo", 587 },
    { "UnableToGetProperty", 588 },
    { "UnableToGetStandardColormap", 589 },
    { "UnableToGetVisual", 590 },
    { "UnableToGrabMouse", 591 },
    { "UnableToLoadFont", 592 },
    { "UnableToMatchVisualToStandardColormap", 593 },
    { "UnableToOpenXServer", 594 },
    { "UnableToReadXAttributes", 595 },
    { "UnableToReadXWindowImage", 596 },
    { "UnrecognizedColormapType", 597 },
    { "UnrecognizedGravityType", 598 },
    { "UnrecognizedVisualSpecifier", 599 },
    { "UnableToAllocateXHints", 600 },
    { "UnableToCreateCursor", 601 },
    { "UnableToCreateGraphicContext", 602 },
    { "UnableToCreateStandardColormap", 603 },
    { "UnableToCreateTextProperty", 604 },
    { "UnableToCreateXImage", 605 },
    { "UnableToCreateXPixmap", 606 },
    { "UnableToCreateXWindow", 607 },
    { "UnableToDisplayImage", 608 },
    { "UnableToDitherImage", 609 },
    { "UnableToGetPixelInfo", 610 },
    { "UnableToGetVisual", 611 },
    { "UnableToLoadFont", 612 },
    { "UnableToMakeXWindow", 613 },
    { "UnableToOpenXServer", 614 },
    { "UnableToViewFonts", 615 },
    { "UnableToGetVisual", 616 },
    { "UsingDefaultVisual", 617 },
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
