#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 611

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
#define MGK_ImageErrorImageColorspaceDiffers 209
#define MGK_ImageErrorImageOpacityDiffers 210
#define MGK_ImageErrorImageSequenceIsRequired 211
#define MGK_ImageErrorImageSizeDiffers 212
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 213
#define MGK_ImageErrorNoImagesWereFound 214
#define MGK_ImageErrorNoImagesWereLoaded 215
#define MGK_ImageErrorNoLocaleImageAttribute 216
#define MGK_ImageErrorTooManyClusters 217
#define MGK_ImageErrorUnableToAppendImage 218
#define MGK_ImageErrorUnableToAverageImage 219
#define MGK_ImageErrorUnableToCoalesceImage 220
#define MGK_ImageErrorUnableToCompareImages 221
#define MGK_ImageErrorUnableToCreateImageMosaic 222
#define MGK_ImageErrorUnableToCreateStereoImage 223
#define MGK_ImageErrorUnableToDeconstructImageSequence 224
#define MGK_ImageErrorUnableToFlattenImage 225
#define MGK_ImageErrorUnableToResizeImage 226
#define MGK_ImageErrorUnableToSegmentImage 227
#define MGK_ImageErrorUnableToSetClipMask 228
#define MGK_ImageErrorUnableToShearImage 229
#define MGK_ImageErrorWidthOrHeightExceedsLimit 230
#define MGK_ImageFatalErrorUnableToPersistKey 231
#define MGK_ImageWarningDefault 232
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 233
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 234
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 235
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 236
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 237
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 238
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 239
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 240
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 241
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 242
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 243
#define MGK_MissingDelegateFatalErrorDefault 244
#define MGK_MissingDelegateWarningDefault 245
#define MGK_ModuleErrorFailedToCloseModule 246
#define MGK_ModuleErrorFailedToFindSymbol 247
#define MGK_ModuleErrorUnableToLoadModule 248
#define MGK_ModuleErrorUnableToRegisterImageFormat 249
#define MGK_ModuleErrorUnrecognizedModule 250
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 251
#define MGK_ModuleWarningDefault 252
#define MGK_OptionErrorBevelWidthIsNegative 253
#define MGK_OptionErrorColorSeparatedImageRequired 254
#define MGK_OptionErrorFrameIsLessThanImageSize 255
#define MGK_OptionErrorGeometryDimensionsAreZero 256
#define MGK_OptionErrorGeometryDoesNotContainImage 257
#define MGK_OptionErrorImagesAreNotTheSameSize 258
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 259
#define MGK_OptionErrorImageSmallerThanKernelWidth 260
#define MGK_OptionErrorImageSmallerThanRadius 261
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 262
#define MGK_OptionErrorInputImagesAlreadySpecified 263
#define MGK_OptionErrorKernelRadiusIsTooSmall 264
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 265
#define MGK_OptionErrorMissingAffineMatrix 266
#define MGK_OptionErrorMissingAmount 267
#define MGK_OptionErrorMissingAmplitude 268
#define MGK_OptionErrorMissingAnImageFilename 269
#define MGK_OptionErrorMissingArgument 270
#define MGK_OptionErrorMissingAzimuth 271
#define MGK_OptionErrorMissingBackgroundColor 272
#define MGK_OptionErrorMissingBevelWidth 273
#define MGK_OptionErrorMissingBorderColor 274
#define MGK_OptionErrorMissingCoefficients 275
#define MGK_OptionErrorMissingColorizeValue 276
#define MGK_OptionErrorMissingColors 277
#define MGK_OptionErrorMissingComment 278
#define MGK_OptionErrorMissingCompressionQuality 279
#define MGK_OptionErrorMissingCycleAmount 280
#define MGK_OptionErrorMissingDelay 281
#define MGK_OptionErrorMissingDisposeMethod 282
#define MGK_OptionErrorMissingDissolveValue 283
#define MGK_OptionErrorMissingDistance 284
#define MGK_OptionErrorMissingDrawingPrimitive 285
#define MGK_OptionErrorMissingEnodingType 286
#define MGK_OptionErrorMissingEventMask 287
#define MGK_OptionErrorMissingFilename 288
#define MGK_OptionErrorMissingFillColor 289
#define MGK_OptionErrorMissingFontName 290
#define MGK_OptionErrorMissingFormatSpecifier 291
#define MGK_OptionErrorMissingGeometry 292
#define MGK_OptionErrorMissingImageDepth 293
#define MGK_OptionErrorMissingImageFrames 294
#define MGK_OptionErrorMissingImageGamma 295
#define MGK_OptionErrorMissingImageIterations 296
#define MGK_OptionErrorMissingImageLabel 297
#define MGK_OptionErrorMissingImageProfile 298
#define MGK_OptionErrorMissingImageRotation 299
#define MGK_OptionErrorMissingImageScene 300
#define MGK_OptionErrorMissingImageType 301
#define MGK_OptionErrorMissingImplodeAmount 302
#define MGK_OptionErrorMissingListName 303
#define MGK_OptionErrorMissingLogFormat 304
#define MGK_OptionErrorMissingMatrix 305
#define MGK_OptionErrorMissingMatteColor 306
#define MGK_OptionErrorMissingOffset 307
#define MGK_OptionErrorMissingOpaqueColor 308
#define MGK_OptionErrorMissingOrder 309
#define MGK_OptionErrorMissingPageGeometry 310
#define MGK_OptionErrorMissingPaintRadius 311
#define MGK_OptionErrorMissingPassword 312
#define MGK_OptionErrorMissingPoint 313
#define MGK_OptionErrorMissingPointsize 314
#define MGK_OptionErrorMissingResourceLimit 315
#define MGK_OptionErrorMissingResourceType 316
#define MGK_OptionErrorMissingSeedValue 317
#define MGK_OptionErrorMissingServerName 318
#define MGK_OptionErrorMissingSpreadAmount 319
#define MGK_OptionErrorMissingThreshold 320
#define MGK_OptionErrorMissingTile 321
#define MGK_OptionErrorMissingTitle 322
#define MGK_OptionErrorMissingTransparentColor 323
#define MGK_OptionErrorMissingTreeDepth 324
#define MGK_OptionErrorMissingType 325
#define MGK_OptionErrorMissingViewTransform 326
#define MGK_OptionErrorMissingVirtualPixelMethod 327
#define MGK_OptionErrorMissingWidth 328
#define MGK_OptionErrorMustSpecifyAnImageName 329
#define MGK_OptionErrorMustSpecifyImageSize 330
#define MGK_OptionErrorNoBlobDefined 331
#define MGK_OptionErrorNoImagesDefined 332
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 333
#define MGK_OptionErrorNoProfileNameWasGiven 334
#define MGK_OptionErrorNullBlobArgument 335
#define MGK_OptionErrorReferenceImageRequired 336
#define MGK_OptionErrorReferenceIsNotMyType 337
#define MGK_OptionErrorSteganoImageRequired 338
#define MGK_OptionErrorStereoImageRequired 339
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 340
#define MGK_OptionErrorUnableToAddOrRemoveProfile 341
#define MGK_OptionErrorUnableToAllocateICCProfile 342
#define MGK_OptionErrorUnableToAverageImageSequence 343
#define MGK_OptionErrorUnableToBlurImage 344
#define MGK_OptionErrorUnableToChopImage 345
#define MGK_OptionErrorUnableToConstituteImage 346
#define MGK_OptionErrorUnableToConvolveImage 347
#define MGK_OptionErrorUnableToEdgeImage 348
#define MGK_OptionErrorUnableToEqualizeImage 349
#define MGK_OptionErrorUnableToFilterImage 350
#define MGK_OptionErrorUnableToFormatImageMetadata 351
#define MGK_OptionErrorUnableToFrameImage 352
#define MGK_OptionErrorUnableToOilPaintImage 353
#define MGK_OptionErrorUnableToPaintImage 354
#define MGK_OptionErrorUnableToRaiseImage 355
#define MGK_OptionErrorUnableToSharpenImage 356
#define MGK_OptionErrorUnableToThresholdImage 357
#define MGK_OptionErrorUnableToWaveImage 358
#define MGK_OptionErrorUnrecognizedAttribute 359
#define MGK_OptionErrorUnrecognizedChannelType 360
#define MGK_OptionErrorUnrecognizedColor 361
#define MGK_OptionErrorUnrecognizedColormapType 362
#define MGK_OptionErrorUnrecognizedColorspace 363
#define MGK_OptionErrorUnrecognizedComposeOperator 364
#define MGK_OptionErrorUnrecognizedDisposeMethod 365
#define MGK_OptionErrorUnrecognizedElement 366
#define MGK_OptionErrorUnrecognizedEndianType 367
#define MGK_OptionErrorUnrecognizedGravityType 368
#define MGK_OptionErrorUnrecognizedImageCompression 369
#define MGK_OptionErrorUnrecognizedImageFilter 370
#define MGK_OptionErrorUnrecognizedImageFormat 371
#define MGK_OptionErrorUnrecognizedImageMode 372
#define MGK_OptionErrorUnrecognizedImageType 373
#define MGK_OptionErrorUnrecognizedIntentType 374
#define MGK_OptionErrorUnrecognizedInterlaceType 375
#define MGK_OptionErrorUnrecognizedListType 376
#define MGK_OptionErrorUnrecognizedModeType 377
#define MGK_OptionErrorUnrecognizedNoiseType 378
#define MGK_OptionErrorUnrecognizedOption 379
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 380
#define MGK_OptionErrorUnrecognizedPixelMap 381
#define MGK_OptionErrorUnrecognizedPreviewType 382
#define MGK_OptionErrorUnrecognizedResourceType 383
#define MGK_OptionErrorUnrecognizedType 384
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 385
#define MGK_OptionErrorUsageError 386
#define MGK_OptionFatalErrorDeprecatedOption 387
#define MGK_OptionFatalErrorInvalidColorspaceType 388
#define MGK_OptionFatalErrorInvalidEndianType 389
#define MGK_OptionFatalErrorInvalidImageType 390
#define MGK_OptionFatalErrorInvalidInterlaceType 391
#define MGK_OptionFatalErrorMissingAnImageFilename 392
#define MGK_OptionFatalErrorMissingBevelWidth 393
#define MGK_OptionFatalErrorMissingBorderColor 394
#define MGK_OptionFatalErrorMissingColor 395
#define MGK_OptionFatalErrorMissingColors 396
#define MGK_OptionFatalErrorMissingCommand 397
#define MGK_OptionFatalErrorMissingComment 398
#define MGK_OptionFatalErrorMissingCompressionQuality 399
#define MGK_OptionFatalErrorMissingDegrees 400
#define MGK_OptionFatalErrorMissingDepth 401
#define MGK_OptionFatalErrorMissingDisposeMethod 402
#define MGK_OptionFatalErrorMissingEncodingType 403
#define MGK_OptionFatalErrorMissingEventMask 404
#define MGK_OptionFatalErrorMissingFactor 405
#define MGK_OptionFatalErrorMissingFilename 406
#define MGK_OptionFatalErrorMissingFontName 407
#define MGK_OptionFatalErrorMissingForeground 408
#define MGK_OptionFatalErrorMissingGeometry 409
#define MGK_OptionFatalErrorMissingIDNameOrRoot 410
#define MGK_OptionFatalErrorMissingImageDepth 411
#define MGK_OptionFatalErrorMissingLabelName 412
#define MGK_OptionFatalErrorMissingLevel 413
#define MGK_OptionFatalErrorMissingLogFormat 414
#define MGK_OptionFatalErrorMissingMapType 415
#define MGK_OptionFatalErrorMissingMethod 416
#define MGK_OptionFatalErrorMissingName 417
#define MGK_OptionFatalErrorMissingPageGeometry 418
#define MGK_OptionFatalErrorMissingPassword 419
#define MGK_OptionFatalErrorMissingQuality 420
#define MGK_OptionFatalErrorMissingResourceLimit 421
#define MGK_OptionFatalErrorMissingResourceType 422
#define MGK_OptionFatalErrorMissingSceneNumber 423
#define MGK_OptionFatalErrorMissingSeconds 424
#define MGK_OptionFatalErrorMissingServerName 425
#define MGK_OptionFatalErrorMissingSize 426
#define MGK_OptionFatalErrorMissingSnaps 427
#define MGK_OptionFatalErrorMissingThreshold 428
#define MGK_OptionFatalErrorMissingTitle 429
#define MGK_OptionFatalErrorMissingType 430
#define MGK_OptionFatalErrorMissingValue 431
#define MGK_OptionFatalErrorMissingVirtualPixelMethod 432
#define MGK_OptionFatalErrorMissingVisualClass 433
#define MGK_OptionFatalErrorMissingWidth 434
#define MGK_OptionFatalErrorNoImagesWereLoaded 435
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 436
#define MGK_OptionFatalErrorUnableToOpenXServer 437
#define MGK_OptionFatalErrorUnableToPersistKey 438
#define MGK_OptionFatalErrorUnrecognizedColormapType 439
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 440
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 441
#define MGK_OptionFatalErrorUnrecognizedEndianType 442
#define MGK_OptionFatalErrorUnrecognizedFilterType 443
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 444
#define MGK_OptionFatalErrorUnrecognizedImageType 445
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 446
#define MGK_OptionFatalErrorUnrecognizedOption 447
#define MGK_OptionFatalErrorUnrecognizedResourceType 448
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 449
#define MGK_OptionWarningUnrecognizedColor 450
#define MGK_RegistryErrorImageExpected 451
#define MGK_RegistryErrorImageInfoExpected 452
#define MGK_RegistryErrorStructureSizeMismatch 453
#define MGK_RegistryErrorUnableToGetRegistryID 454
#define MGK_RegistryErrorUnableToLocateImage 455
#define MGK_RegistryErrorUnableToSetRegistry 456
#define MGK_RegistryFatalErrorDefault 457
#define MGK_RegistryWarningDefault 458
#define MGK_ResourceLimitErrorCacheResourcesExhausted 459
#define MGK_ResourceLimitErrorMemoryAllocationFailed 460
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 461
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 462
#define MGK_ResourceLimitErrorUnableToAddColorProfile 463
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 464
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 465
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 466
#define MGK_ResourceLimitErrorUnableToAllocateColormap 467
#define MGK_ResourceLimitErrorUnableToAllocateString 468
#define MGK_ResourceLimitErrorUnableToAnnotateImage 469
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 470
#define MGK_ResourceLimitErrorUnableToCloneImage 471
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 472
#define MGK_ResourceLimitErrorUnableToConstituteImage 473
#define MGK_ResourceLimitErrorUnableToConvertFont 474
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 475
#define MGK_ResourceLimitErrorUnableToCreateColormap 476
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 477
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 478
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 479
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 480
#define MGK_ResourceLimitErrorUnableToCreateXWindow 481
#define MGK_ResourceLimitErrorUnableToCropImage 482
#define MGK_ResourceLimitErrorUnableToDespeckleImage 483
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 484
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 485
#define MGK_ResourceLimitErrorUnableToDitherImage 486
#define MGK_ResourceLimitErrorUnableToDrawOnImage 487
#define MGK_ResourceLimitErrorUnableToEdgeImage 488
#define MGK_ResourceLimitErrorUnableToEmbossImage 489
#define MGK_ResourceLimitErrorUnableToEnhanceImage 490
#define MGK_ResourceLimitErrorUnableToFloodfillImage 491
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 492
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 493
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 494
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 495
#define MGK_ResourceLimitErrorUnableToLevelImage 496
#define MGK_ResourceLimitErrorUnableToMagnifyImage 497
#define MGK_ResourceLimitErrorUnableToManageColor 498
#define MGK_ResourceLimitErrorUnableToMapImage 499
#define MGK_ResourceLimitErrorUnableToMapImageSequence 500
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 501
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 502
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 503
#define MGK_ResourceLimitErrorUnableToNormalizeImage 504
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 505
#define MGK_ResourceLimitErrorUnableToQuantizeImage 506
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 507
#define MGK_ResourceLimitErrorUnableToReadTextChunk 508
#define MGK_ResourceLimitErrorUnableToReadXImage 509
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 510
#define MGK_ResourceLimitErrorUnableToResizeImage 511
#define MGK_ResourceLimitErrorUnableToRotateImage 512
#define MGK_ResourceLimitErrorUnableToSampleImage 513
#define MGK_ResourceLimitErrorUnableToScaleImage 514
#define MGK_ResourceLimitErrorUnableToSelectImage 515
#define MGK_ResourceLimitErrorUnableToSharpenImage 516
#define MGK_ResourceLimitErrorUnableToShaveImage 517
#define MGK_ResourceLimitErrorUnableToShearImage 518
#define MGK_ResourceLimitErrorUnableToSortImageColormap 519
#define MGK_ResourceLimitErrorUnableToThresholdImage 520
#define MGK_ResourceLimitErrorUnableToTransformColorspace 521
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 522
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 523
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 524
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 525
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 526
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 527
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 528
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 529
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 530
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 531
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 532
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 533
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 534
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 535
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 536
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 537
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 538
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 539
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 540
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 541
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 542
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 543
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 544
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 545
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 546
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 547
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 548
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 549
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 550
#define MGK_ResourceLimitFatalErrorUnableToConvertText 551
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 552
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 553
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 554
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 555
#define MGK_ResourceLimitWarningMemoryAllocationFailed 556
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 557
#define MGK_StreamErrorNoStreamHandlerIsDefined 558
#define MGK_StreamErrorPixelCacheIsNotOpen 559
#define MGK_StreamErrorUnableToAcquirePixelStream 560
#define MGK_StreamErrorUnableToSetPixelStream 561
#define MGK_StreamErrorUnableToSyncPixelStream 562
#define MGK_StreamFatalErrorDefault 563
#define MGK_StreamWarningDefault 564
#define MGK_TypeErrorFontSubstitutionRequired 565
#define MGK_TypeErrorUnableToGetTypeMetrics 566
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 567
#define MGK_TypeErrorUnableToReadFont 568
#define MGK_TypeErrorUnrecognizedFontEncoding 569
#define MGK_TypeFatalErrorDefault 570
#define MGK_TypeWarningDefault 571
#define MGK_XServerErrorColorIsNotKnownToServer 572
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 573
#define MGK_XServerErrorStandardColormapIsNotInitialized 574
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 575
#define MGK_XServerErrorUnableToCreateBitmap 576
#define MGK_XServerErrorUnableToCreateColormap 577
#define MGK_XServerErrorUnableToCreatePixmap 578
#define MGK_XServerErrorUnableToCreateProperty 579
#define MGK_XServerErrorUnableToCreateStandardColormap 580
#define MGK_XServerErrorUnableToDisplayImageInfo 581
#define MGK_XServerErrorUnableToGetProperty 582
#define MGK_XServerErrorUnableToGetStandardColormap 583
#define MGK_XServerErrorUnableToGetVisual 584
#define MGK_XServerErrorUnableToGrabMouse 585
#define MGK_XServerErrorUnableToLoadFont 586
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 587
#define MGK_XServerErrorUnableToOpenXServer 588
#define MGK_XServerErrorUnableToReadXAttributes 589
#define MGK_XServerErrorUnableToReadXWindowImage 590
#define MGK_XServerErrorUnrecognizedColormapType 591
#define MGK_XServerErrorUnrecognizedGravityType 592
#define MGK_XServerErrorUnrecognizedVisualSpecifier 593
#define MGK_XServerFatalErrorUnableToAllocateXHints 594
#define MGK_XServerFatalErrorUnableToCreateCursor 595
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 596
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 597
#define MGK_XServerFatalErrorUnableToCreateTextProperty 598
#define MGK_XServerFatalErrorUnableToCreateXImage 599
#define MGK_XServerFatalErrorUnableToCreateXPixmap 600
#define MGK_XServerFatalErrorUnableToCreateXWindow 601
#define MGK_XServerFatalErrorUnableToDisplayImage 602
#define MGK_XServerFatalErrorUnableToDitherImage 603
#define MGK_XServerFatalErrorUnableToGetPixelInfo 604
#define MGK_XServerFatalErrorUnableToGetVisual 605
#define MGK_XServerFatalErrorUnableToLoadFont 606
#define MGK_XServerFatalErrorUnableToMakeXWindow 607
#define MGK_XServerFatalErrorUnableToOpenXServer 608
#define MGK_XServerFatalErrorUnableToViewFonts 609
#define MGK_XServerWarningUnableToGetVisual 610
#define MGK_XServerWarningUsingDefaultVisual 611

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
