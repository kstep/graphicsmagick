#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 613

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
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 214
#define MGK_ImageErrorNoImagesWereFound 215
#define MGK_ImageErrorNoImagesWereLoaded 216
#define MGK_ImageErrorNoLocaleImageAttribute 217
#define MGK_ImageErrorTooManyClusters 218
#define MGK_ImageErrorUnableToAppendImage 219
#define MGK_ImageErrorUnableToAssignProfile 220
#define MGK_ImageErrorUnableToAverageImage 221
#define MGK_ImageErrorUnableToCoalesceImage 222
#define MGK_ImageErrorUnableToCompareImages 223
#define MGK_ImageErrorUnableToCreateImageMosaic 224
#define MGK_ImageErrorUnableToCreateStereoImage 225
#define MGK_ImageErrorUnableToDeconstructImageSequence 226
#define MGK_ImageErrorUnableToFlattenImage 227
#define MGK_ImageErrorUnableToResizeImage 228
#define MGK_ImageErrorUnableToSegmentImage 229
#define MGK_ImageErrorUnableToSetClipMask 230
#define MGK_ImageErrorUnableToShearImage 231
#define MGK_ImageErrorWidthOrHeightExceedsLimit 232
#define MGK_ImageFatalErrorUnableToPersistKey 233
#define MGK_ImageWarningDefault 234
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 235
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 236
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 237
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 238
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 239
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 240
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 241
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 242
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 243
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 244
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 245
#define MGK_MissingDelegateFatalErrorDefault 246
#define MGK_MissingDelegateWarningDefault 247
#define MGK_ModuleErrorFailedToCloseModule 248
#define MGK_ModuleErrorFailedToFindSymbol 249
#define MGK_ModuleErrorUnableToLoadModule 250
#define MGK_ModuleErrorUnableToRegisterImageFormat 251
#define MGK_ModuleErrorUnrecognizedModule 252
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 253
#define MGK_ModuleWarningDefault 254
#define MGK_OptionErrorBevelWidthIsNegative 255
#define MGK_OptionErrorColorSeparatedImageRequired 256
#define MGK_OptionErrorFrameIsLessThanImageSize 257
#define MGK_OptionErrorGeometryDimensionsAreZero 258
#define MGK_OptionErrorGeometryDoesNotContainImage 259
#define MGK_OptionErrorImagesAreNotTheSameSize 260
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 261
#define MGK_OptionErrorImageSmallerThanKernelWidth 262
#define MGK_OptionErrorImageSmallerThanRadius 263
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 264
#define MGK_OptionErrorInputImagesAlreadySpecified 265
#define MGK_OptionErrorKernelRadiusIsTooSmall 266
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 267
#define MGK_OptionErrorMissingAffineMatrix 268
#define MGK_OptionErrorMissingAmount 269
#define MGK_OptionErrorMissingAmplitude 270
#define MGK_OptionErrorMissingAnImageFilename 271
#define MGK_OptionErrorMissingArgument 272
#define MGK_OptionErrorMissingAzimuth 273
#define MGK_OptionErrorMissingBackgroundColor 274
#define MGK_OptionErrorMissingBevelWidth 275
#define MGK_OptionErrorMissingBorderColor 276
#define MGK_OptionErrorMissingCoefficients 277
#define MGK_OptionErrorMissingColorizeValue 278
#define MGK_OptionErrorMissingColors 279
#define MGK_OptionErrorMissingComment 280
#define MGK_OptionErrorMissingCompressionQuality 281
#define MGK_OptionErrorMissingCycleAmount 282
#define MGK_OptionErrorMissingDelay 283
#define MGK_OptionErrorMissingDisposeMethod 284
#define MGK_OptionErrorMissingDissolveValue 285
#define MGK_OptionErrorMissingDistance 286
#define MGK_OptionErrorMissingDrawingPrimitive 287
#define MGK_OptionErrorMissingEnodingType 288
#define MGK_OptionErrorMissingEventMask 289
#define MGK_OptionErrorMissingFilename 290
#define MGK_OptionErrorMissingFillColor 291
#define MGK_OptionErrorMissingFontName 292
#define MGK_OptionErrorMissingFormatSpecifier 293
#define MGK_OptionErrorMissingGeometry 294
#define MGK_OptionErrorMissingImageDepth 295
#define MGK_OptionErrorMissingImageFrames 296
#define MGK_OptionErrorMissingImageGamma 297
#define MGK_OptionErrorMissingImageIterations 298
#define MGK_OptionErrorMissingImageLabel 299
#define MGK_OptionErrorMissingImageProfile 300
#define MGK_OptionErrorMissingImageRotation 301
#define MGK_OptionErrorMissingImageScene 302
#define MGK_OptionErrorMissingImageType 303
#define MGK_OptionErrorMissingImplodeAmount 304
#define MGK_OptionErrorMissingListName 305
#define MGK_OptionErrorMissingLogFormat 306
#define MGK_OptionErrorMissingMatrix 307
#define MGK_OptionErrorMissingMatteColor 308
#define MGK_OptionErrorMissingOffset 309
#define MGK_OptionErrorMissingOpaqueColor 310
#define MGK_OptionErrorMissingOrder 311
#define MGK_OptionErrorMissingPageGeometry 312
#define MGK_OptionErrorMissingPaintRadius 313
#define MGK_OptionErrorMissingPassword 314
#define MGK_OptionErrorMissingPoint 315
#define MGK_OptionErrorMissingPointsize 316
#define MGK_OptionErrorMissingResourceLimit 317
#define MGK_OptionErrorMissingResourceType 318
#define MGK_OptionErrorMissingSeedValue 319
#define MGK_OptionErrorMissingServerName 320
#define MGK_OptionErrorMissingSpreadAmount 321
#define MGK_OptionErrorMissingThreshold 322
#define MGK_OptionErrorMissingTile 323
#define MGK_OptionErrorMissingTitle 324
#define MGK_OptionErrorMissingTransparentColor 325
#define MGK_OptionErrorMissingTreeDepth 326
#define MGK_OptionErrorMissingType 327
#define MGK_OptionErrorMissingViewTransform 328
#define MGK_OptionErrorMissingVirtualPixelMethod 329
#define MGK_OptionErrorMissingWidth 330
#define MGK_OptionErrorMustSpecifyAnImageName 331
#define MGK_OptionErrorMustSpecifyImageSize 332
#define MGK_OptionErrorNoBlobDefined 333
#define MGK_OptionErrorNoImagesDefined 334
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 335
#define MGK_OptionErrorNoProfileNameWasGiven 336
#define MGK_OptionErrorNullBlobArgument 337
#define MGK_OptionErrorReferenceImageRequired 338
#define MGK_OptionErrorReferenceIsNotMyType 339
#define MGK_OptionErrorSteganoImageRequired 340
#define MGK_OptionErrorStereoImageRequired 341
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 342
#define MGK_OptionErrorUnableToAddOrRemoveProfile 343
#define MGK_OptionErrorUnableToAllocateICCProfile 344
#define MGK_OptionErrorUnableToAverageImageSequence 345
#define MGK_OptionErrorUnableToBlurImage 346
#define MGK_OptionErrorUnableToChopImage 347
#define MGK_OptionErrorUnableToConstituteImage 348
#define MGK_OptionErrorUnableToConvolveImage 349
#define MGK_OptionErrorUnableToEdgeImage 350
#define MGK_OptionErrorUnableToEqualizeImage 351
#define MGK_OptionErrorUnableToFilterImage 352
#define MGK_OptionErrorUnableToFormatImageMetadata 353
#define MGK_OptionErrorUnableToFrameImage 354
#define MGK_OptionErrorUnableToOilPaintImage 355
#define MGK_OptionErrorUnableToPaintImage 356
#define MGK_OptionErrorUnableToRaiseImage 357
#define MGK_OptionErrorUnableToSharpenImage 358
#define MGK_OptionErrorUnableToThresholdImage 359
#define MGK_OptionErrorUnableToWaveImage 360
#define MGK_OptionErrorUnrecognizedAttribute 361
#define MGK_OptionErrorUnrecognizedChannelType 362
#define MGK_OptionErrorUnrecognizedColor 363
#define MGK_OptionErrorUnrecognizedColormapType 364
#define MGK_OptionErrorUnrecognizedColorspace 365
#define MGK_OptionErrorUnrecognizedComposeOperator 366
#define MGK_OptionErrorUnrecognizedDisposeMethod 367
#define MGK_OptionErrorUnrecognizedElement 368
#define MGK_OptionErrorUnrecognizedEndianType 369
#define MGK_OptionErrorUnrecognizedGravityType 370
#define MGK_OptionErrorUnrecognizedImageCompression 371
#define MGK_OptionErrorUnrecognizedImageFilter 372
#define MGK_OptionErrorUnrecognizedImageFormat 373
#define MGK_OptionErrorUnrecognizedImageMode 374
#define MGK_OptionErrorUnrecognizedImageType 375
#define MGK_OptionErrorUnrecognizedIntentType 376
#define MGK_OptionErrorUnrecognizedInterlaceType 377
#define MGK_OptionErrorUnrecognizedListType 378
#define MGK_OptionErrorUnrecognizedModeType 379
#define MGK_OptionErrorUnrecognizedNoiseType 380
#define MGK_OptionErrorUnrecognizedOption 381
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 382
#define MGK_OptionErrorUnrecognizedPixelMap 383
#define MGK_OptionErrorUnrecognizedPreviewType 384
#define MGK_OptionErrorUnrecognizedResourceType 385
#define MGK_OptionErrorUnrecognizedType 386
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 387
#define MGK_OptionErrorUsageError 388
#define MGK_OptionFatalErrorDeprecatedOption 389
#define MGK_OptionFatalErrorInvalidColorspaceType 390
#define MGK_OptionFatalErrorInvalidEndianType 391
#define MGK_OptionFatalErrorInvalidImageType 392
#define MGK_OptionFatalErrorInvalidInterlaceType 393
#define MGK_OptionFatalErrorMissingAnImageFilename 394
#define MGK_OptionFatalErrorMissingBevelWidth 395
#define MGK_OptionFatalErrorMissingBorderColor 396
#define MGK_OptionFatalErrorMissingColor 397
#define MGK_OptionFatalErrorMissingColors 398
#define MGK_OptionFatalErrorMissingCommand 399
#define MGK_OptionFatalErrorMissingComment 400
#define MGK_OptionFatalErrorMissingCompressionQuality 401
#define MGK_OptionFatalErrorMissingDegrees 402
#define MGK_OptionFatalErrorMissingDepth 403
#define MGK_OptionFatalErrorMissingDisposeMethod 404
#define MGK_OptionFatalErrorMissingEncodingType 405
#define MGK_OptionFatalErrorMissingEventMask 406
#define MGK_OptionFatalErrorMissingFactor 407
#define MGK_OptionFatalErrorMissingFilename 408
#define MGK_OptionFatalErrorMissingFontName 409
#define MGK_OptionFatalErrorMissingForeground 410
#define MGK_OptionFatalErrorMissingGeometry 411
#define MGK_OptionFatalErrorMissingIDNameOrRoot 412
#define MGK_OptionFatalErrorMissingImageDepth 413
#define MGK_OptionFatalErrorMissingLabelName 414
#define MGK_OptionFatalErrorMissingLevel 415
#define MGK_OptionFatalErrorMissingLogFormat 416
#define MGK_OptionFatalErrorMissingMapType 417
#define MGK_OptionFatalErrorMissingMethod 418
#define MGK_OptionFatalErrorMissingName 419
#define MGK_OptionFatalErrorMissingPageGeometry 420
#define MGK_OptionFatalErrorMissingPassword 421
#define MGK_OptionFatalErrorMissingQuality 422
#define MGK_OptionFatalErrorMissingResourceLimit 423
#define MGK_OptionFatalErrorMissingResourceType 424
#define MGK_OptionFatalErrorMissingSceneNumber 425
#define MGK_OptionFatalErrorMissingSeconds 426
#define MGK_OptionFatalErrorMissingServerName 427
#define MGK_OptionFatalErrorMissingSize 428
#define MGK_OptionFatalErrorMissingSnaps 429
#define MGK_OptionFatalErrorMissingThreshold 430
#define MGK_OptionFatalErrorMissingTitle 431
#define MGK_OptionFatalErrorMissingType 432
#define MGK_OptionFatalErrorMissingValue 433
#define MGK_OptionFatalErrorMissingVirtualPixelMethod 434
#define MGK_OptionFatalErrorMissingVisualClass 435
#define MGK_OptionFatalErrorMissingWidth 436
#define MGK_OptionFatalErrorNoImagesWereLoaded 437
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 438
#define MGK_OptionFatalErrorUnableToOpenXServer 439
#define MGK_OptionFatalErrorUnableToPersistKey 440
#define MGK_OptionFatalErrorUnrecognizedColormapType 441
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 442
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 443
#define MGK_OptionFatalErrorUnrecognizedEndianType 444
#define MGK_OptionFatalErrorUnrecognizedFilterType 445
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 446
#define MGK_OptionFatalErrorUnrecognizedImageType 447
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 448
#define MGK_OptionFatalErrorUnrecognizedOption 449
#define MGK_OptionFatalErrorUnrecognizedResourceType 450
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 451
#define MGK_OptionWarningUnrecognizedColor 452
#define MGK_RegistryErrorImageExpected 453
#define MGK_RegistryErrorImageInfoExpected 454
#define MGK_RegistryErrorStructureSizeMismatch 455
#define MGK_RegistryErrorUnableToGetRegistryID 456
#define MGK_RegistryErrorUnableToLocateImage 457
#define MGK_RegistryErrorUnableToSetRegistry 458
#define MGK_RegistryFatalErrorDefault 459
#define MGK_RegistryWarningDefault 460
#define MGK_ResourceLimitErrorCacheResourcesExhausted 461
#define MGK_ResourceLimitErrorMemoryAllocationFailed 462
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 463
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 464
#define MGK_ResourceLimitErrorUnableToAddColorProfile 465
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 466
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 467
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 468
#define MGK_ResourceLimitErrorUnableToAllocateColormap 469
#define MGK_ResourceLimitErrorUnableToAllocateString 470
#define MGK_ResourceLimitErrorUnableToAnnotateImage 471
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 472
#define MGK_ResourceLimitErrorUnableToCloneImage 473
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 474
#define MGK_ResourceLimitErrorUnableToConstituteImage 475
#define MGK_ResourceLimitErrorUnableToConvertFont 476
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 477
#define MGK_ResourceLimitErrorUnableToCreateColormap 478
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 479
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 480
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 481
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 482
#define MGK_ResourceLimitErrorUnableToCreateXWindow 483
#define MGK_ResourceLimitErrorUnableToCropImage 484
#define MGK_ResourceLimitErrorUnableToDespeckleImage 485
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 486
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 487
#define MGK_ResourceLimitErrorUnableToDitherImage 488
#define MGK_ResourceLimitErrorUnableToDrawOnImage 489
#define MGK_ResourceLimitErrorUnableToEdgeImage 490
#define MGK_ResourceLimitErrorUnableToEmbossImage 491
#define MGK_ResourceLimitErrorUnableToEnhanceImage 492
#define MGK_ResourceLimitErrorUnableToFloodfillImage 493
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 494
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 495
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 496
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 497
#define MGK_ResourceLimitErrorUnableToLevelImage 498
#define MGK_ResourceLimitErrorUnableToMagnifyImage 499
#define MGK_ResourceLimitErrorUnableToManageColor 500
#define MGK_ResourceLimitErrorUnableToMapImage 501
#define MGK_ResourceLimitErrorUnableToMapImageSequence 502
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 503
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 504
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 505
#define MGK_ResourceLimitErrorUnableToNormalizeImage 506
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 507
#define MGK_ResourceLimitErrorUnableToQuantizeImage 508
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 509
#define MGK_ResourceLimitErrorUnableToReadTextChunk 510
#define MGK_ResourceLimitErrorUnableToReadXImage 511
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 512
#define MGK_ResourceLimitErrorUnableToResizeImage 513
#define MGK_ResourceLimitErrorUnableToRotateImage 514
#define MGK_ResourceLimitErrorUnableToSampleImage 515
#define MGK_ResourceLimitErrorUnableToScaleImage 516
#define MGK_ResourceLimitErrorUnableToSelectImage 517
#define MGK_ResourceLimitErrorUnableToSharpenImage 518
#define MGK_ResourceLimitErrorUnableToShaveImage 519
#define MGK_ResourceLimitErrorUnableToShearImage 520
#define MGK_ResourceLimitErrorUnableToSortImageColormap 521
#define MGK_ResourceLimitErrorUnableToThresholdImage 522
#define MGK_ResourceLimitErrorUnableToTransformColorspace 523
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 524
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 525
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 526
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 527
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 528
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 529
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 530
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 531
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 532
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 533
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 534
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 535
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 536
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 537
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 538
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 539
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 540
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 541
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 542
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 543
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 544
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 545
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 546
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 547
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 548
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 549
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 550
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 551
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 552
#define MGK_ResourceLimitFatalErrorUnableToConvertText 553
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 554
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 555
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 556
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 557
#define MGK_ResourceLimitWarningMemoryAllocationFailed 558
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 559
#define MGK_StreamErrorNoStreamHandlerIsDefined 560
#define MGK_StreamErrorPixelCacheIsNotOpen 561
#define MGK_StreamErrorUnableToAcquirePixelStream 562
#define MGK_StreamErrorUnableToSetPixelStream 563
#define MGK_StreamErrorUnableToSyncPixelStream 564
#define MGK_StreamFatalErrorDefault 565
#define MGK_StreamWarningDefault 566
#define MGK_TypeErrorFontSubstitutionRequired 567
#define MGK_TypeErrorUnableToGetTypeMetrics 568
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 569
#define MGK_TypeErrorUnableToReadFont 570
#define MGK_TypeErrorUnrecognizedFontEncoding 571
#define MGK_TypeFatalErrorDefault 572
#define MGK_TypeWarningDefault 573
#define MGK_XServerErrorColorIsNotKnownToServer 574
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 575
#define MGK_XServerErrorStandardColormapIsNotInitialized 576
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 577
#define MGK_XServerErrorUnableToCreateBitmap 578
#define MGK_XServerErrorUnableToCreateColormap 579
#define MGK_XServerErrorUnableToCreatePixmap 580
#define MGK_XServerErrorUnableToCreateProperty 581
#define MGK_XServerErrorUnableToCreateStandardColormap 582
#define MGK_XServerErrorUnableToDisplayImageInfo 583
#define MGK_XServerErrorUnableToGetProperty 584
#define MGK_XServerErrorUnableToGetStandardColormap 585
#define MGK_XServerErrorUnableToGetVisual 586
#define MGK_XServerErrorUnableToGrabMouse 587
#define MGK_XServerErrorUnableToLoadFont 588
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 589
#define MGK_XServerErrorUnableToOpenXServer 590
#define MGK_XServerErrorUnableToReadXAttributes 591
#define MGK_XServerErrorUnableToReadXWindowImage 592
#define MGK_XServerErrorUnrecognizedColormapType 593
#define MGK_XServerErrorUnrecognizedGravityType 594
#define MGK_XServerErrorUnrecognizedVisualSpecifier 595
#define MGK_XServerFatalErrorUnableToAllocateXHints 596
#define MGK_XServerFatalErrorUnableToCreateCursor 597
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 598
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 599
#define MGK_XServerFatalErrorUnableToCreateTextProperty 600
#define MGK_XServerFatalErrorUnableToCreateXImage 601
#define MGK_XServerFatalErrorUnableToCreateXPixmap 602
#define MGK_XServerFatalErrorUnableToCreateXWindow 603
#define MGK_XServerFatalErrorUnableToDisplayImage 604
#define MGK_XServerFatalErrorUnableToDitherImage 605
#define MGK_XServerFatalErrorUnableToGetPixelInfo 606
#define MGK_XServerFatalErrorUnableToGetVisual 607
#define MGK_XServerFatalErrorUnableToLoadFont 608
#define MGK_XServerFatalErrorUnableToMakeXWindow 609
#define MGK_XServerFatalErrorUnableToOpenXServer 610
#define MGK_XServerFatalErrorUnableToViewFonts 611
#define MGK_XServerWarningUnableToGetVisual 612
#define MGK_XServerWarningUsingDefaultVisual 613

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
    { "Image/FatalError", 232, ImageFatalError },
    { "Image/Warning", 233, ImageWarning },
    { "Missing/Delegate/Error", 234, MissingDelegateError },
    { "Missing/Delegate/FatalError", 245, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 246, MissingDelegateWarning },
    { "Module/Error", 247, ModuleError },
    { "Module/FatalError", 252, ModuleFatalError },
    { "Module/Warning", 253, ModuleWarning },
    { "Option/Error", 254, OptionError },
    { "Option/FatalError", 388, OptionFatalError },
    { "Option/Warning", 451, OptionWarning },
    { "Registry/Error", 452, RegistryError },
    { "Registry/FatalError", 458, RegistryFatalError },
    { "Registry/Warning", 459, RegistryWarning },
    { "Resource/Limit/Error", 460, ResourceLimitError },
    { "Resource/Limit/FatalError", 523, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 557, ResourceLimitWarning },
    { "Stream/Error", 558, StreamError },
    { "Stream/FatalError", 564, StreamFatalError },
    { "Stream/Warning", 565, StreamWarning },
    { "Type/Error", 566, TypeError },
    { "Type/FatalError", 571, TypeFatalError },
    { "Type/Warning", 572, TypeWarning },
    { "XServer/Error", 573, XServerError },
    { "XServer/FatalError", 595, XServerFatalError },
    { "XServer/Warning", 611, XServerWarning },
    { 0, 613, 0 }
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
    { "LeftAndRightImageSizesDiffer", 214 },
    { "NoImagesWereFound", 215 },
    { "NoImagesWereLoaded", 216 },
    { "NoLocaleImageAttribute", 217 },
    { "TooManyClusters", 218 },
    { "UnableToAppendImage", 219 },
    { "UnableToAssignProfile", 220 },
    { "UnableToAverageImage", 221 },
    { "UnableToCoalesceImage", 222 },
    { "UnableToCompareImages", 223 },
    { "UnableToCreateImageMosaic", 224 },
    { "UnableToCreateStereoImage", 225 },
    { "UnableToDeconstructImageSequence", 226 },
    { "UnableToFlattenImage", 227 },
    { "UnableToResizeImage", 228 },
    { "UnableToSegmentImage", 229 },
    { "UnableToSetClipMask", 230 },
    { "UnableToShearImage", 231 },
    { "WidthOrHeightExceedsLimit", 232 },
    { "UnableToPersistKey", 233 },
    { "Default", 234 },
    { "DPSLibraryIsNotAvailable", 235 },
    { "FPXLibraryIsNotAvailable", 236 },
    { "FreeTypeLibraryIsNotAvailable", 237 },
    { "JPEGLibraryIsNotAvailable", 238 },
    { "LZWEncodingNotEnabled", 239 },
    { "NoDecodeDelegateForThisImageFormat", 240 },
    { "NoEncodeDelegateForThisImageFormat", 241 },
    { "TIFFLibraryIsNotAvailable", 242 },
    { "XMLLibraryIsNotAvailable", 243 },
    { "XWindowLibraryIsNotAvailable", 244 },
    { "ZipLibraryIsNotAvailable", 245 },
    { "Default", 246 },
    { "Default", 247 },
    { "FailedToCloseModule", 248 },
    { "FailedToFindSymbol", 249 },
    { "UnableToLoadModule", 250 },
    { "UnableToRegisterImageFormat", 251 },
    { "UnrecognizedModule", 252 },
    { "UnableToInitializeModuleLoader", 253 },
    { "Default", 254 },
    { "BevelWidthIsNegative", 255 },
    { "ColorSeparatedImageRequired", 256 },
    { "FrameIsLessThanImageSize", 257 },
    { "GeometryDimensionsAreZero", 258 },
    { "GeometryDoesNotContainImage", 259 },
    { "ImagesAreNotTheSameSize", 260 },
    { "ImageSizeMustExceedBevelWidth", 261 },
    { "ImageSmallerThanKernelWidth", 262 },
    { "ImageSmallerThanRadius", 263 },
    { "ImageWidthsOrHeightsDiffer", 264 },
    { "InputImagesAlreadySpecified", 265 },
    { "KernelRadiusIsTooSmall", 266 },
    { "KernelWidthMustBeAnOddNumber", 267 },
    { "MissingAffineMatrix", 268 },
    { "MissingAmount", 269 },
    { "MissingAmplitude", 270 },
    { "MissingAnImageFilename", 271 },
    { "MissingArgument", 272 },
    { "MissingAzimuth", 273 },
    { "MissingBackgroundColor", 274 },
    { "MissingBevelWidth", 275 },
    { "MissingBorderColor", 276 },
    { "MissingCoefficients", 277 },
    { "MissingColorizeValue", 278 },
    { "MissingColors", 279 },
    { "MissingComment", 280 },
    { "MissingCompressionQuality", 281 },
    { "MissingCycleAmount", 282 },
    { "MissingDelay", 283 },
    { "MissingDisposeMethod", 284 },
    { "MissingDissolveValue", 285 },
    { "MissingDistance", 286 },
    { "MissingDrawingPrimitive", 287 },
    { "MissingEnodingType", 288 },
    { "MissingEventMask", 289 },
    { "MissingFilename", 290 },
    { "MissingFillColor", 291 },
    { "MissingFontName", 292 },
    { "MissingFormatSpecifier", 293 },
    { "MissingGeometry", 294 },
    { "MissingImageDepth", 295 },
    { "MissingImageFrames", 296 },
    { "MissingImageGamma", 297 },
    { "MissingImageIterations", 298 },
    { "MissingImageLabel", 299 },
    { "MissingImageProfile", 300 },
    { "MissingImageRotation", 301 },
    { "MissingImageScene", 302 },
    { "MissingImageType", 303 },
    { "MissingImplodeAmount", 304 },
    { "MissingListName", 305 },
    { "MissingLogFormat", 306 },
    { "MissingMatrix", 307 },
    { "MissingMatteColor", 308 },
    { "MissingOffset", 309 },
    { "MissingOpaqueColor", 310 },
    { "MissingOrder", 311 },
    { "MissingPageGeometry", 312 },
    { "MissingPaintRadius", 313 },
    { "MissingPassword", 314 },
    { "MissingPoint", 315 },
    { "MissingPointsize", 316 },
    { "MissingResourceLimit", 317 },
    { "MissingResourceType", 318 },
    { "MissingSeedValue", 319 },
    { "MissingServerName", 320 },
    { "MissingSpreadAmount", 321 },
    { "MissingThreshold", 322 },
    { "MissingTile", 323 },
    { "MissingTitle", 324 },
    { "MissingTransparentColor", 325 },
    { "MissingTreeDepth", 326 },
    { "MissingType", 327 },
    { "MissingViewTransform", 328 },
    { "MissingVirtualPixelMethod", 329 },
    { "MissingWidth", 330 },
    { "MustSpecifyAnImageName", 331 },
    { "MustSpecifyImageSize", 332 },
    { "NoBlobDefined", 333 },
    { "NoImagesDefined", 334 },
    { "NonzeroWidthAndHeightRequired", 335 },
    { "NoProfileNameWasGiven", 336 },
    { "NullBlobArgument", 337 },
    { "ReferenceImageRequired", 338 },
    { "ReferenceIsNotMyType", 339 },
    { "SteganoImageRequired", 340 },
    { "StereoImageRequired", 341 },
    { "SubimageSpecificationReturnsNoImages", 342 },
    { "UnableToAddOrRemoveProfile", 343 },
    { "UnableToAllocateICCProfile", 344 },
    { "UnableToAverageImageSequence", 345 },
    { "UnableToBlurImage", 346 },
    { "UnableToChopImage", 347 },
    { "UnableToConstituteImage", 348 },
    { "UnableToConvolveImage", 349 },
    { "UnableToEdgeImage", 350 },
    { "UnableToEqualizeImage", 351 },
    { "UnableToFilterImage", 352 },
    { "UnableToFormatImageMetadata", 353 },
    { "UnableToFrameImage", 354 },
    { "UnableToOilPaintImage", 355 },
    { "UnableToPaintImage", 356 },
    { "UnableToRaiseImage", 357 },
    { "UnableToSharpenImage", 358 },
    { "UnableToThresholdImage", 359 },
    { "UnableToWaveImage", 360 },
    { "UnrecognizedAttribute", 361 },
    { "UnrecognizedChannelType", 362 },
    { "UnrecognizedColor", 363 },
    { "UnrecognizedColormapType", 364 },
    { "UnrecognizedColorspace", 365 },
    { "UnrecognizedComposeOperator", 366 },
    { "UnrecognizedDisposeMethod", 367 },
    { "UnrecognizedElement", 368 },
    { "UnrecognizedEndianType", 369 },
    { "UnrecognizedGravityType", 370 },
    { "UnrecognizedImageCompression", 371 },
    { "UnrecognizedImageFilter", 372 },
    { "UnrecognizedImageFormat", 373 },
    { "UnrecognizedImageMode", 374 },
    { "UnrecognizedImageType", 375 },
    { "UnrecognizedIntentType", 376 },
    { "UnrecognizedInterlaceType", 377 },
    { "UnrecognizedListType", 378 },
    { "UnrecognizedModeType", 379 },
    { "UnrecognizedNoiseType", 380 },
    { "UnrecognizedOption", 381 },
    { "UnrecognizedPerlMagickMethod", 382 },
    { "UnrecognizedPixelMap", 383 },
    { "UnrecognizedPreviewType", 384 },
    { "UnrecognizedResourceType", 385 },
    { "UnrecognizedType", 386 },
    { "UnrecognizedVirtualPixelMethod", 387 },
    { "UsageError", 388 },
    { "DeprecatedOption", 389 },
    { "InvalidColorspaceType", 390 },
    { "InvalidEndianType", 391 },
    { "InvalidImageType", 392 },
    { "InvalidInterlaceType", 393 },
    { "MissingAnImageFilename", 394 },
    { "MissingBevelWidth", 395 },
    { "MissingBorderColor", 396 },
    { "MissingColor", 397 },
    { "MissingColors", 398 },
    { "MissingCommand", 399 },
    { "MissingComment", 400 },
    { "MissingCompressionQuality", 401 },
    { "MissingDegrees", 402 },
    { "MissingDepth", 403 },
    { "MissingDisposeMethod", 404 },
    { "MissingEncodingType", 405 },
    { "MissingEventMask", 406 },
    { "MissingFactor", 407 },
    { "MissingFilename", 408 },
    { "MissingFontName", 409 },
    { "MissingForeground", 410 },
    { "MissingGeometry", 411 },
    { "MissingIDNameOrRoot", 412 },
    { "MissingImageDepth", 413 },
    { "MissingLabelName", 414 },
    { "MissingLevel", 415 },
    { "MissingLogFormat", 416 },
    { "MissingMapType", 417 },
    { "MissingMethod", 418 },
    { "MissingName", 419 },
    { "MissingPageGeometry", 420 },
    { "MissingPassword", 421 },
    { "MissingQuality", 422 },
    { "MissingResourceLimit", 423 },
    { "MissingResourceType", 424 },
    { "MissingSceneNumber", 425 },
    { "MissingSeconds", 426 },
    { "MissingServerName", 427 },
    { "MissingSize", 428 },
    { "MissingSnaps", 429 },
    { "MissingThreshold", 430 },
    { "MissingTitle", 431 },
    { "MissingType", 432 },
    { "MissingValue", 433 },
    { "MissingVirtualPixelMethod", 434 },
    { "MissingVisualClass", 435 },
    { "MissingWidth", 436 },
    { "NoImagesWereLoaded", 437 },
    { "OptionLengthExceedsLimit", 438 },
    { "UnableToOpenXServer", 439 },
    { "UnableToPersistKey", 440 },
    { "UnrecognizedColormapType", 441 },
    { "UnrecognizedColorspaceType", 442 },
    { "UnrecognizedDisposeMethod", 443 },
    { "UnrecognizedEndianType", 444 },
    { "UnrecognizedFilterType", 445 },
    { "UnrecognizedImageCompressionType", 446 },
    { "UnrecognizedImageType", 447 },
    { "UnrecognizedInterlaceType", 448 },
    { "UnrecognizedOption", 449 },
    { "UnrecognizedResourceType", 450 },
    { "UnrecognizedVirtualPixelMethod", 451 },
    { "UnrecognizedColor", 452 },
    { "ImageExpected", 453 },
    { "ImageInfoExpected", 454 },
    { "StructureSizeMismatch", 455 },
    { "UnableToGetRegistryID", 456 },
    { "UnableToLocateImage", 457 },
    { "UnableToSetRegistry", 458 },
    { "Default", 459 },
    { "Default", 460 },
    { "CacheResourcesExhausted", 461 },
    { "MemoryAllocationFailed", 462 },
    { "NoPixelsDefinedInCache", 463 },
    { "PixelCacheAllocationFailed", 464 },
    { "UnableToAddColorProfile", 465 },
    { "UnableToAddGenericProfile", 466 },
    { "UnableToAddIPTCProfile", 467 },
    { "UnableToAllocateCoefficients", 468 },
    { "UnableToAllocateColormap", 469 },
    { "UnableToAllocateString", 470 },
    { "UnableToAnnotateImage", 471 },
    { "UnableToAverageImageSequence", 472 },
    { "UnableToCloneImage", 473 },
    { "UnableToComputeImageSignature", 474 },
    { "UnableToConstituteImage", 475 },
    { "UnableToConvertFont", 476 },
    { "UnableToConvertStringToTokens", 477 },
    { "UnableToCreateColormap", 478 },
    { "UnableToCreateColorTransform", 479 },
    { "UnableToCreateCommandWidget", 480 },
    { "UnableToCreateImageGroup", 481 },
    { "UnableToCreateImageMontage", 482 },
    { "UnableToCreateXWindow", 483 },
    { "UnableToCropImage", 484 },
    { "UnableToDespeckleImage", 485 },
    { "UnableToDetermineImageClass", 486 },
    { "UnableToDetermineTheNumberOfImageColors", 487 },
    { "UnableToDitherImage", 488 },
    { "UnableToDrawOnImage", 489 },
    { "UnableToEdgeImage", 490 },
    { "UnableToEmbossImage", 491 },
    { "UnableToEnhanceImage", 492 },
    { "UnableToFloodfillImage", 493 },
    { "UnableToGammaCorrectImage", 494 },
    { "UnableToGetBestIconSize", 495 },
    { "UnableToGetFromRegistry", 496 },
    { "UnableToGetPackageInfo", 497 },
    { "UnableToLevelImage", 498 },
    { "UnableToMagnifyImage", 499 },
    { "UnableToManageColor", 500 },
    { "UnableToMapImage", 501 },
    { "UnableToMapImageSequence", 502 },
    { "UnableToMedianFilterImage", 503 },
    { "UnableToMotionBlurImage", 504 },
    { "UnableToNoiseFilterImage", 505 },
    { "UnableToNormalizeImage", 506 },
    { "UnableToOpenColorProfile", 507 },
    { "UnableToQuantizeImage", 508 },
    { "UnableToQuantizeImageSequence", 509 },
    { "UnableToReadTextChunk", 510 },
    { "UnableToReadXImage", 511 },
    { "UnableToReadXServerColormap", 512 },
    { "UnableToResizeImage", 513 },
    { "UnableToRotateImage", 514 },
    { "UnableToSampleImage", 515 },
    { "UnableToScaleImage", 516 },
    { "UnableToSelectImage", 517 },
    { "UnableToSharpenImage", 518 },
    { "UnableToShaveImage", 519 },
    { "UnableToShearImage", 520 },
    { "UnableToSortImageColormap", 521 },
    { "UnableToThresholdImage", 522 },
    { "UnableToTransformColorspace", 523 },
    { "MemoryAllocationFailed", 524 },
    { "UnableToAllocateAscii85Info", 525 },
    { "UnableToAllocateCacheInfo", 526 },
    { "UnableToAllocateCacheView", 527 },
    { "UnableToAllocateColorInfo", 528 },
    { "UnableToAllocateDashPattern", 529 },
    { "UnableToAllocateDelegateInfo", 530 },
    { "UnableToAllocateDerivatives", 531 },
    { "UnableToAllocateDrawContext", 532 },
    { "UnableToAllocateDrawInfo", 533 },
    { "UnableToAllocateGammaMap", 534 },
    { "UnableToAllocateImage", 535 },
    { "UnableToAllocateImagePixels", 536 },
    { "UnableToAllocateLogInfo", 537 },
    { "UnableToAllocateMagicInfo", 538 },
    { "UnableToAllocateMagickInfo", 539 },
    { "UnableToAllocateModuleInfo", 540 },
    { "UnableToAllocateMontageInfo", 541 },
    { "UnableToAllocateQuantizeInfo", 542 },
    { "UnableToAllocateRegistryInfo", 543 },
    { "UnableToAllocateSemaphoreInfo", 544 },
    { "UnableToAllocateString", 545 },
    { "UnableToAllocateTypeInfo", 546 },
    { "UnableToAnimateImageSequence", 547 },
    { "UnableToCloneBlobInfo", 548 },
    { "UnableToCloneCacheInfo", 549 },
    { "UnableToCloneImage", 550 },
    { "UnableToCloneImageInfo", 551 },
    { "UnableToConcatenateString", 552 },
    { "UnableToConvertText", 553 },
    { "UnableToCreateColormap", 554 },
    { "UnableToDisplayImage", 555 },
    { "UnableToEscapeString", 556 },
    { "UnableToInterpretMSLImage", 557 },
    { "MemoryAllocationFailed", 558 },
    { "ImageDoesNotContainTheStreamGeometry", 559 },
    { "NoStreamHandlerIsDefined", 560 },
    { "PixelCacheIsNotOpen", 561 },
    { "UnableToAcquirePixelStream", 562 },
    { "UnableToSetPixelStream", 563 },
    { "UnableToSyncPixelStream", 564 },
    { "Default", 565 },
    { "Default", 566 },
    { "FontSubstitutionRequired", 567 },
    { "UnableToGetTypeMetrics", 568 },
    { "UnableToInitializeFreetypeLibrary", 569 },
    { "UnableToReadFont", 570 },
    { "UnrecognizedFontEncoding", 571 },
    { "Default", 572 },
    { "Default", 573 },
    { "ColorIsNotKnownToServer", 574 },
    { "NoWindowWithSpecifiedIDExists", 575 },
    { "StandardColormapIsNotInitialized", 576 },
    { "UnableToConnectToRemoteDisplay", 577 },
    { "UnableToCreateBitmap", 578 },
    { "UnableToCreateColormap", 579 },
    { "UnableToCreatePixmap", 580 },
    { "UnableToCreateProperty", 581 },
    { "UnableToCreateStandardColormap", 582 },
    { "UnableToDisplayImageInfo", 583 },
    { "UnableToGetProperty", 584 },
    { "UnableToGetStandardColormap", 585 },
    { "UnableToGetVisual", 586 },
    { "UnableToGrabMouse", 587 },
    { "UnableToLoadFont", 588 },
    { "UnableToMatchVisualToStandardColormap", 589 },
    { "UnableToOpenXServer", 590 },
    { "UnableToReadXAttributes", 591 },
    { "UnableToReadXWindowImage", 592 },
    { "UnrecognizedColormapType", 593 },
    { "UnrecognizedGravityType", 594 },
    { "UnrecognizedVisualSpecifier", 595 },
    { "UnableToAllocateXHints", 596 },
    { "UnableToCreateCursor", 597 },
    { "UnableToCreateGraphicContext", 598 },
    { "UnableToCreateStandardColormap", 599 },
    { "UnableToCreateTextProperty", 600 },
    { "UnableToCreateXImage", 601 },
    { "UnableToCreateXPixmap", 602 },
    { "UnableToCreateXWindow", 603 },
    { "UnableToDisplayImage", 604 },
    { "UnableToDitherImage", 605 },
    { "UnableToGetPixelInfo", 606 },
    { "UnableToGetVisual", 607 },
    { "UnableToLoadFont", 608 },
    { "UnableToMakeXWindow", 609 },
    { "UnableToOpenXServer", 610 },
    { "UnableToViewFonts", 611 },
    { "UnableToGetVisual", 612 },
    { "UsingDefaultVisual", 613 },
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
