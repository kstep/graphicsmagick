#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 542

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
#define MGK_CoderErrorUnableToWriteTemporaryFile 84
#define MGK_CoderErrorUnableToZipCompressImage 85
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 86
#define MGK_CoderErrorWebPDecodingFailedUserAbort 87
#define MGK_CoderErrorWebPEncodingFailed 88
#define MGK_CoderErrorWebPEncodingFailedBadDimension 89
#define MGK_CoderErrorWebPEncodingFailedBadWrite 90
#define MGK_CoderErrorWebPEncodingFailedBitstreamOutOfMemory 91
#define MGK_CoderErrorWebPEncodingFailedFileTooBig 92
#define MGK_CoderErrorWebPEncodingFailedInvalidConfiguration 93
#define MGK_CoderErrorWebPEncodingFailedNULLParameter 94
#define MGK_CoderErrorWebPEncodingFailedOutOfMemory 95
#define MGK_CoderErrorWebPEncodingFailedPartition0Overflow 96
#define MGK_CoderErrorWebPEncodingFailedPartitionOverflow 97
#define MGK_CoderErrorWebPEncodingFailedUserAbort 98
#define MGK_CoderErrorWebPInvalidConfiguration 99
#define MGK_CoderErrorWebPInvalidParameter 100
#define MGK_CoderErrorZipCompressionNotSupported 101
#define MGK_CoderFatalErrorDefault 102
#define MGK_CoderWarningLosslessToLossyJPEGConversion 103
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 104
#define MGK_ConfigureErrorRegistryKeyLookupFailed 105
#define MGK_ConfigureErrorStringTokenLengthExceeded 106
#define MGK_ConfigureErrorUnableToAccessConfigureFile 107
#define MGK_ConfigureErrorUnableToAccessFontFile 108
#define MGK_ConfigureErrorUnableToAccessLogFile 109
#define MGK_ConfigureErrorUnableToAccessModuleFile 110
#define MGK_ConfigureFatalErrorDefault 111
#define MGK_ConfigureFatalErrorUnableToChangeToWorkingDirectory 112
#define MGK_ConfigureFatalErrorUnableToGetCurrentDirectory 113
#define MGK_ConfigureFatalErrorUnableToRestoreCurrentDirectory 114
#define MGK_ConfigureWarningDefault 115
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 116
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 117
#define MGK_CorruptImageErrorColormapExceedsColorsLimit 118
#define MGK_CorruptImageErrorCompressionNotValid 119
#define MGK_CorruptImageErrorCorruptImage 120
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 121
#define MGK_CorruptImageErrorImageTypeNotSupported 122
#define MGK_CorruptImageErrorImproperImageHeader 123
#define MGK_CorruptImageErrorInsufficientImageDataInFile 124
#define MGK_CorruptImageErrorInvalidColormapIndex 125
#define MGK_CorruptImageErrorInvalidFileFormatVersion 126
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 127
#define MGK_CorruptImageErrorMissingImageChannel 128
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 129
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 130
#define MGK_CorruptImageErrorNotEnoughTiles 131
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 132
#define MGK_CorruptImageErrorSubsamplingRequiresEvenWidth 133
#define MGK_CorruptImageErrorTooMuchImageDataInFile 134
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 135
#define MGK_CorruptImageErrorUnableToReadColorProfile 136
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 137
#define MGK_CorruptImageErrorUnableToReadGenericProfile 138
#define MGK_CorruptImageErrorUnableToReadImageData 139
#define MGK_CorruptImageErrorUnableToReadImageHeader 140
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 141
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 142
#define MGK_CorruptImageErrorUnableToReadSubImageData 143
#define MGK_CorruptImageErrorUnableToReadVIDImage 144
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 145
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 146
#define MGK_CorruptImageErrorUnableToUncompressImage 147
#define MGK_CorruptImageErrorUnexpectedEndOfFile 148
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 149
#define MGK_CorruptImageErrorUnknownPatternType 150
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 151
#define MGK_CorruptImageErrorUnrecognizedImageCompression 152
#define MGK_CorruptImageErrorUnrecognizedNumberOfColors 153
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 154
#define MGK_CorruptImageFatalErrorUnableToPersistKey 155
#define MGK_CorruptImageWarningCompressionNotValid 156
#define MGK_CorruptImageWarningImproperImageHeader 157
#define MGK_CorruptImageWarningLengthAndFilesizeDoNotMatch 158
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 159
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 160
#define MGK_CorruptImageWarningSkipToSyncByte 161
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 162
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 163
#define MGK_CorruptImageWarningUnrecognizedImageCompression 164
#define MGK_DelegateErrorDelegateFailed 165
#define MGK_DelegateErrorFailedToAllocateArgumentList 166
#define MGK_DelegateErrorFailedToAllocateGhostscriptInterpreter 167
#define MGK_DelegateErrorFailedToComputeOutputSize 168
#define MGK_DelegateErrorFailedToFindGhostscript 169
#define MGK_DelegateErrorFailedToRenderFile 170
#define MGK_DelegateErrorFailedToScanFile 171
#define MGK_DelegateErrorNoTagFound 172
#define MGK_DelegateErrorPostscriptDelegateFailed 173
#define MGK_DelegateErrorUnableToCreateImage 174
#define MGK_DelegateErrorUnableToCreateImageComponent 175
#define MGK_DelegateErrorUnableToDecodeImageFile 176
#define MGK_DelegateErrorUnableToEncodeImageFile 177
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 178
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 179
#define MGK_DelegateErrorUnableToManageJP2Stream 180
#define MGK_DelegateErrorUnableToWriteSVGFormat 181
#define MGK_DelegateErrorWebPABIMismatch 182
#define MGK_DelegateFatalErrorDefault 183
#define MGK_DelegateWarningDefault 184
#define MGK_DrawErrorAlreadyPushingPatternDefinition 185
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 186
#define MGK_DrawErrorTooManyCoordinates 187
#define MGK_DrawErrorUnableToPrint 188
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 189
#define MGK_DrawFatalErrorDefault 190
#define MGK_DrawWarningNotARelativeURL 191
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 192
#define MGK_DrawWarningURLNotFound 193
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 194
#define MGK_FileOpenErrorUnableToOpenFile 195
#define MGK_FileOpenErrorUnableToWriteFile 196
#define MGK_FileOpenFatalErrorDefault 197
#define MGK_FileOpenWarningDefault 198
#define MGK_ImageErrorAngleIsDiscontinuous 199
#define MGK_ImageErrorColorspaceColorProfileMismatch 200
#define MGK_ImageErrorImageColorspaceDiffers 201
#define MGK_ImageErrorImageColorspaceMismatch 202
#define MGK_ImageErrorImageDifferenceExceedsLimit 203
#define MGK_ImageErrorImageDoesNotContainResolution 204
#define MGK_ImageErrorImageOpacityDiffers 205
#define MGK_ImageErrorImageSequenceIsRequired 206
#define MGK_ImageErrorImageSizeDiffers 207
#define MGK_ImageErrorInvalidColormapIndex 208
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 209
#define MGK_ImageErrorNoImagesWereFound 210
#define MGK_ImageErrorNoImagesWereLoaded 211
#define MGK_ImageErrorNoLocaleImageAttribute 212
#define MGK_ImageErrorTooManyClusters 213
#define MGK_ImageErrorUnableToAppendImage 214
#define MGK_ImageErrorUnableToAssignProfile 215
#define MGK_ImageErrorUnableToAverageImage 216
#define MGK_ImageErrorUnableToCoalesceImage 217
#define MGK_ImageErrorUnableToCompareImages 218
#define MGK_ImageErrorUnableToCreateImageMosaic 219
#define MGK_ImageErrorUnableToCreateStereoImage 220
#define MGK_ImageErrorUnableToDeconstructImageSequence 221
#define MGK_ImageErrorUnableToFlattenImage 222
#define MGK_ImageErrorUnableToGetClipMask 223
#define MGK_ImageErrorUnableToHandleImageChannel 224
#define MGK_ImageErrorUnableToResizeImage 225
#define MGK_ImageErrorUnableToSegmentImage 226
#define MGK_ImageErrorUnableToSetClipMask 227
#define MGK_ImageErrorUnableToShearImage 228
#define MGK_ImageErrorWidthOrHeightExceedsLimit 229
#define MGK_ImageFatalErrorUnableToPersistKey 230
#define MGK_ImageWarningDefault 231
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 232
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 233
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 234
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 235
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 236
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
#define MGK_MonitorErrorDefault 253
#define MGK_MonitorFatalErrorDefault 254
#define MGK_MonitorFatalErrorUserRequestedTerminationBySignal 255
#define MGK_MonitorWarningDefault 256
#define MGK_OptionErrorBevelWidthIsNegative 257
#define MGK_OptionErrorColorSeparatedImageRequired 258
#define MGK_OptionErrorFrameIsLessThanImageSize 259
#define MGK_OptionErrorGeometryDimensionsAreZero 260
#define MGK_OptionErrorGeometryDoesNotContainImage 261
#define MGK_OptionErrorHaldClutImageDimensionsInvalid 262
#define MGK_OptionErrorImagesAreNotTheSameSize 263
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 264
#define MGK_OptionErrorImageSmallerThanKernelWidth 265
#define MGK_OptionErrorImageSmallerThanRadius 266
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 267
#define MGK_OptionErrorInputImagesAlreadySpecified 268
#define MGK_OptionErrorInvalidSubimageSpecification 269
#define MGK_OptionErrorKernelRadiusIsTooSmall 270
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 271
#define MGK_OptionErrorMatrixIsNotSquare 272
#define MGK_OptionErrorMatrixOrderOutOfRange 273
#define MGK_OptionErrorMissingAnImageFilename 274
#define MGK_OptionErrorMissingArgument 275
#define MGK_OptionErrorMustSpecifyAnImageName 276
#define MGK_OptionErrorMustSpecifyImageSize 277
#define MGK_OptionErrorNoBlobDefined 278
#define MGK_OptionErrorNoImagesDefined 279
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 280
#define MGK_OptionErrorNoProfileNameWasGiven 281
#define MGK_OptionErrorNullBlobArgument 282
#define MGK_OptionErrorReferenceImageRequired 283
#define MGK_OptionErrorReferenceIsNotMyType 284
#define MGK_OptionErrorRegionAreaExceedsLimit 285
#define MGK_OptionErrorRequestDidNotReturnAnImage 286
#define MGK_OptionErrorSteganoImageRequired 287
#define MGK_OptionErrorStereoImageRequired 288
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 289
#define MGK_OptionErrorUnableToAddOrRemoveProfile 290
#define MGK_OptionErrorUnableToAverageImageSequence 291
#define MGK_OptionErrorUnableToBlurImage 292
#define MGK_OptionErrorUnableToChopImage 293
#define MGK_OptionErrorUnableToColorMatrixImage 294
#define MGK_OptionErrorUnableToConstituteImage 295
#define MGK_OptionErrorUnableToConvolveImage 296
#define MGK_OptionErrorUnableToEdgeImage 297
#define MGK_OptionErrorUnableToEqualizeImage 298
#define MGK_OptionErrorUnableToFilterImage 299
#define MGK_OptionErrorUnableToFormatImageMetadata 300
#define MGK_OptionErrorUnableToFrameImage 301
#define MGK_OptionErrorUnableToOilPaintImage 302
#define MGK_OptionErrorUnableToPaintImage 303
#define MGK_OptionErrorUnableToRaiseImage 304
#define MGK_OptionErrorUnableToSharpenImage 305
#define MGK_OptionErrorUnableToThresholdImage 306
#define MGK_OptionErrorUnableToWaveImage 307
#define MGK_OptionErrorUnrecognizedAttribute 308
#define MGK_OptionErrorUnrecognizedChannelType 309
#define MGK_OptionErrorUnrecognizedColor 310
#define MGK_OptionErrorUnrecognizedColormapType 311
#define MGK_OptionErrorUnrecognizedColorspace 312
#define MGK_OptionErrorUnrecognizedCommand 313
#define MGK_OptionErrorUnrecognizedComposeOperator 314
#define MGK_OptionErrorUnrecognizedDisposeMethod 315
#define MGK_OptionErrorUnrecognizedElement 316
#define MGK_OptionErrorUnrecognizedEndianType 317
#define MGK_OptionErrorUnrecognizedGravityType 318
#define MGK_OptionErrorUnrecognizedHighlightStyle 319
#define MGK_OptionErrorUnrecognizedImageCompression 320
#define MGK_OptionErrorUnrecognizedImageFilter 321
#define MGK_OptionErrorUnrecognizedImageFormat 322
#define MGK_OptionErrorUnrecognizedImageMode 323
#define MGK_OptionErrorUnrecognizedImageType 324
#define MGK_OptionErrorUnrecognizedIntentType 325
#define MGK_OptionErrorUnrecognizedInterlaceType 326
#define MGK_OptionErrorUnrecognizedListType 327
#define MGK_OptionErrorUnrecognizedMetric 328
#define MGK_OptionErrorUnrecognizedModeType 329
#define MGK_OptionErrorUnrecognizedNoiseType 330
#define MGK_OptionErrorUnrecognizedOperator 331
#define MGK_OptionErrorUnrecognizedOption 332
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 333
#define MGK_OptionErrorUnrecognizedPixelMap 334
#define MGK_OptionErrorUnrecognizedPreviewType 335
#define MGK_OptionErrorUnrecognizedResourceType 336
#define MGK_OptionErrorUnrecognizedType 337
#define MGK_OptionErrorUnrecognizedUnitsType 338
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 339
#define MGK_OptionErrorUnsupportedSamplingFactor 340
#define MGK_OptionErrorUsageError 341
#define MGK_OptionFatalErrorInvalidColorspaceType 342
#define MGK_OptionFatalErrorInvalidEndianType 343
#define MGK_OptionFatalErrorInvalidImageType 344
#define MGK_OptionFatalErrorInvalidInterlaceType 345
#define MGK_OptionFatalErrorMissingAnImageFilename 346
#define MGK_OptionFatalErrorMissingArgument 347
#define MGK_OptionFatalErrorNoImagesWereLoaded 348
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 349
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 350
#define MGK_OptionFatalErrorUnableToOpenXServer 351
#define MGK_OptionFatalErrorUnableToPersistKey 352
#define MGK_OptionFatalErrorUnrecognizedColormapType 353
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 354
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 355
#define MGK_OptionFatalErrorUnrecognizedEndianType 356
#define MGK_OptionFatalErrorUnrecognizedFilterType 357
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 358
#define MGK_OptionFatalErrorUnrecognizedImageType 359
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 360
#define MGK_OptionFatalErrorUnrecognizedOption 361
#define MGK_OptionFatalErrorUnrecognizedResourceType 362
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 363
#define MGK_OptionWarningUnrecognizedColor 364
#define MGK_RegistryErrorImageExpected 365
#define MGK_RegistryErrorImageInfoExpected 366
#define MGK_RegistryErrorStructureSizeMismatch 367
#define MGK_RegistryErrorUnableToGetRegistryID 368
#define MGK_RegistryErrorUnableToLocateImage 369
#define MGK_RegistryErrorUnableToSetRegistry 370
#define MGK_RegistryFatalErrorDefault 371
#define MGK_RegistryWarningDefault 372
#define MGK_ResourceLimitErrorCacheResourcesExhausted 373
#define MGK_ResourceLimitErrorImagePixelHeightLimitExceeded 374
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 375
#define MGK_ResourceLimitErrorImagePixelWidthLimitExceeded 376
#define MGK_ResourceLimitErrorMemoryAllocationFailed 377
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 378
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 379
#define MGK_ResourceLimitErrorUnableToAddColorProfile 380
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 381
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 382
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 383
#define MGK_ResourceLimitErrorUnableToAllocateColormap 384
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 385
#define MGK_ResourceLimitErrorUnableToAllocateString 386
#define MGK_ResourceLimitErrorUnableToAnnotateImage 387
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 388
#define MGK_ResourceLimitErrorUnableToCloneDrawingWand 389
#define MGK_ResourceLimitErrorUnableToCloneImage 390
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 391
#define MGK_ResourceLimitErrorUnableToConstituteImage 392
#define MGK_ResourceLimitErrorUnableToConvertFont 393
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 394
#define MGK_ResourceLimitErrorUnableToCreateColormap 395
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 396
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 397
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 398
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 399
#define MGK_ResourceLimitErrorUnableToCreateXWindow 400
#define MGK_ResourceLimitErrorUnableToCropImage 401
#define MGK_ResourceLimitErrorUnableToDespeckleImage 402
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 403
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 404
#define MGK_ResourceLimitErrorUnableToDitherImage 405
#define MGK_ResourceLimitErrorUnableToDrawOnImage 406
#define MGK_ResourceLimitErrorUnableToEdgeImage 407
#define MGK_ResourceLimitErrorUnableToEmbossImage 408
#define MGK_ResourceLimitErrorUnableToEnhanceImage 409
#define MGK_ResourceLimitErrorUnableToFloodfillImage 410
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 411
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 412
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 413
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 414
#define MGK_ResourceLimitErrorUnableToLevelImage 415
#define MGK_ResourceLimitErrorUnableToMagnifyImage 416
#define MGK_ResourceLimitErrorUnableToManageColor 417
#define MGK_ResourceLimitErrorUnableToMapImage 418
#define MGK_ResourceLimitErrorUnableToMapImageSequence 419
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 420
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 421
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 422
#define MGK_ResourceLimitErrorUnableToNormalizeImage 423
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 424
#define MGK_ResourceLimitErrorUnableToQuantizeImage 425
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 426
#define MGK_ResourceLimitErrorUnableToReadTextChunk 427
#define MGK_ResourceLimitErrorUnableToReadXImage 428
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 429
#define MGK_ResourceLimitErrorUnableToResizeImage 430
#define MGK_ResourceLimitErrorUnableToRotateImage 431
#define MGK_ResourceLimitErrorUnableToSampleImage 432
#define MGK_ResourceLimitErrorUnableToScaleImage 433
#define MGK_ResourceLimitErrorUnableToSelectImage 434
#define MGK_ResourceLimitErrorUnableToSharpenImage 435
#define MGK_ResourceLimitErrorUnableToShaveImage 436
#define MGK_ResourceLimitErrorUnableToShearImage 437
#define MGK_ResourceLimitErrorUnableToSortImageColormap 438
#define MGK_ResourceLimitErrorUnableToThresholdImage 439
#define MGK_ResourceLimitErrorUnableToTransformColorspace 440
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 441
#define MGK_ResourceLimitFatalErrorSemaporeOperationFailed 442
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 443
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 444
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 445
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 446
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 447
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 448
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 449
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 450
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 451
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 452
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 453
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 454
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 455
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 456
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 457
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 458
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 459
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 460
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 461
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 462
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 463
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 464
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 465
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 466
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 467
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 468
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 469
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 470
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 471
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 472
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 473
#define MGK_ResourceLimitFatalErrorUnableToConvertText 474
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 475
#define MGK_ResourceLimitFatalErrorUnableToDestroySemaphore 476
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 477
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 478
#define MGK_ResourceLimitFatalErrorUnableToInitializeSemaphore 479
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 480
#define MGK_ResourceLimitFatalErrorUnableToLockSemaphore 481
#define MGK_ResourceLimitFatalErrorUnableToUnlockSemaphore 482
#define MGK_ResourceLimitWarningMemoryAllocationFailed 483
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 484
#define MGK_StreamErrorNoStreamHandlerIsDefined 485
#define MGK_StreamErrorPixelCacheIsNotOpen 486
#define MGK_StreamErrorUnableToAcquirePixelStream 487
#define MGK_StreamErrorUnableToSetPixelStream 488
#define MGK_StreamErrorUnableToSyncPixelStream 489
#define MGK_StreamFatalErrorDefault 490
#define MGK_StreamWarningDefault 491
#define MGK_TypeErrorFontSubstitutionRequired 492
#define MGK_TypeErrorUnableToGetTypeMetrics 493
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 494
#define MGK_TypeErrorUnableToReadFont 495
#define MGK_TypeErrorUnrecognizedFontEncoding 496
#define MGK_TypeFatalErrorDefault 497
#define MGK_TypeWarningDefault 498
#define MGK_WandErrorInvalidColormapIndex 499
#define MGK_WandErrorWandAPINotImplemented 500
#define MGK_WandErrorWandContainsNoImageIndexs 501
#define MGK_WandErrorWandContainsNoImages 502
#define MGK_XServerErrorColorIsNotKnownToServer 503
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 504
#define MGK_XServerErrorStandardColormapIsNotInitialized 505
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 506
#define MGK_XServerErrorUnableToCreateBitmap 507
#define MGK_XServerErrorUnableToCreateColormap 508
#define MGK_XServerErrorUnableToCreatePixmap 509
#define MGK_XServerErrorUnableToCreateProperty 510
#define MGK_XServerErrorUnableToCreateStandardColormap 511
#define MGK_XServerErrorUnableToDisplayImageInfo 512
#define MGK_XServerErrorUnableToGetProperty 513
#define MGK_XServerErrorUnableToGetStandardColormap 514
#define MGK_XServerErrorUnableToGetVisual 515
#define MGK_XServerErrorUnableToGrabMouse 516
#define MGK_XServerErrorUnableToLoadFont 517
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 518
#define MGK_XServerErrorUnableToOpenXServer 519
#define MGK_XServerErrorUnableToReadXAttributes 520
#define MGK_XServerErrorUnableToReadXWindowImage 521
#define MGK_XServerErrorUnrecognizedColormapType 522
#define MGK_XServerErrorUnrecognizedGravityType 523
#define MGK_XServerErrorUnrecognizedVisualSpecifier 524
#define MGK_XServerFatalErrorUnableToAllocateXHints 525
#define MGK_XServerFatalErrorUnableToCreateCursor 526
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 527
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 528
#define MGK_XServerFatalErrorUnableToCreateTextProperty 529
#define MGK_XServerFatalErrorUnableToCreateXImage 530
#define MGK_XServerFatalErrorUnableToCreateXPixmap 531
#define MGK_XServerFatalErrorUnableToCreateXWindow 532
#define MGK_XServerFatalErrorUnableToDisplayImage 533
#define MGK_XServerFatalErrorUnableToDitherImage 534
#define MGK_XServerFatalErrorUnableToGetPixelInfo 535
#define MGK_XServerFatalErrorUnableToGetVisual 536
#define MGK_XServerFatalErrorUnableToLoadFont 537
#define MGK_XServerFatalErrorUnableToMakeXWindow 538
#define MGK_XServerFatalErrorUnableToOpenXServer 539
#define MGK_XServerFatalErrorUnableToViewFonts 540
#define MGK_XServerWarningUnableToGetVisual 541
#define MGK_XServerWarningUsingDefaultVisual 542

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
    { "Coder/FatalError", 101, CoderFatalError },
    { "Coder/Warning", 102, CoderWarning },
    { "Configure/Error", 103, ConfigureError },
    { "Configure/FatalError", 110, ConfigureFatalError },
    { "Configure/Warning", 114, ConfigureWarning },
    { "Corrupt/Image/Error", 115, CorruptImageError },
    { "Corrupt/Image/FatalError", 154, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 155, CorruptImageWarning },
    { "Delegate/Error", 164, DelegateError },
    { "Delegate/FatalError", 182, DelegateFatalError },
    { "Delegate/Warning", 183, DelegateWarning },
    { "Draw/Error", 184, DrawError },
    { "Draw/FatalError", 189, DrawFatalError },
    { "Draw/Warning", 190, DrawWarning },
    { "File/Open/Error", 193, FileOpenError },
    { "File/Open/FatalError", 196, FileOpenFatalError },
    { "File/Open/Warning", 197, FileOpenWarning },
    { "Image/Error", 198, ImageError },
    { "Image/FatalError", 229, ImageFatalError },
    { "Image/Warning", 230, ImageWarning },
    { "Missing/Delegate/Error", 231, MissingDelegateError },
    { "Missing/Delegate/FatalError", 243, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 244, MissingDelegateWarning },
    { "Module/Error", 245, ModuleError },
    { "Module/FatalError", 250, ModuleFatalError },
    { "Module/Warning", 251, ModuleWarning },
    { "Monitor/Error", 252, MonitorError },
    { "Monitor/FatalError", 253, MonitorFatalError },
    { "Monitor/Warning", 255, MonitorWarning },
    { "Option/Error", 256, OptionError },
    { "Option/FatalError", 341, OptionFatalError },
    { "Option/Warning", 363, OptionWarning },
    { "Registry/Error", 364, RegistryError },
    { "Registry/FatalError", 370, RegistryFatalError },
    { "Registry/Warning", 371, RegistryWarning },
    { "Resource/Limit/Error", 372, ResourceLimitError },
    { "Resource/Limit/FatalError", 440, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 482, ResourceLimitWarning },
    { "Stream/Error", 483, StreamError },
    { "Stream/FatalError", 489, StreamFatalError },
    { "Stream/Warning", 490, StreamWarning },
    { "Type/Error", 491, TypeError },
    { "Type/FatalError", 496, TypeFatalError },
    { "Type/Warning", 497, TypeWarning },
    { "Wand/Error", 498, WandError },
    { "XServer/Error", 502, XServerError },
    { "XServer/FatalError", 524, XServerFatalError },
    { "XServer/Warning", 540, XServerWarning },
    { 0, 542, UndefinedException }
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
    { "UnableToWriteTemporaryFile", 84 },
    { "UnableToZipCompressImage", 85 },
    { "UnsupportedCellTypeInTheMatrix", 86 },
    { "WebPDecodingFailedUserAbort", 87 },
    { "WebPEncodingFailed", 88 },
    { "WebPEncodingFailedBadDimension", 89 },
    { "WebPEncodingFailedBadWrite", 90 },
    { "WebPEncodingFailedBitstreamOutOfMemory", 91 },
    { "WebPEncodingFailedFileTooBig", 92 },
    { "WebPEncodingFailedInvalidConfiguration", 93 },
    { "WebPEncodingFailedNULLParameter", 94 },
    { "WebPEncodingFailedOutOfMemory", 95 },
    { "WebPEncodingFailedPartition0Overflow", 96 },
    { "WebPEncodingFailedPartitionOverflow", 97 },
    { "WebPEncodingFailedUserAbort", 98 },
    { "WebPInvalidConfiguration", 99 },
    { "WebPInvalidParameter", 100 },
    { "ZipCompressionNotSupported", 101 },
    { "Default", 102 },
    { "LosslessToLossyJPEGConversion", 103 },
    { "IncludeElementNestedTooDeeply", 104 },
    { "RegistryKeyLookupFailed", 105 },
    { "StringTokenLengthExceeded", 106 },
    { "UnableToAccessConfigureFile", 107 },
    { "UnableToAccessFontFile", 108 },
    { "UnableToAccessLogFile", 109 },
    { "UnableToAccessModuleFile", 110 },
    { "Default", 111 },
    { "UnableToChangeToWorkingDirectory", 112 },
    { "UnableToGetCurrentDirectory", 113 },
    { "UnableToRestoreCurrentDirectory", 114 },
    { "Default", 115 },
    { "AnErrorHasOccurredReadingFromFile", 116 },
    { "AnErrorHasOccurredWritingToFile", 117 },
    { "ColormapExceedsColorsLimit", 118 },
    { "CompressionNotValid", 119 },
    { "CorruptImage", 120 },
    { "ImageFileDoesNotContainAnyImageData", 121 },
    { "ImageTypeNotSupported", 122 },
    { "ImproperImageHeader", 123 },
    { "InsufficientImageDataInFile", 124 },
    { "InvalidColormapIndex", 125 },
    { "InvalidFileFormatVersion", 126 },
    { "LengthAndFilesizeDoNotMatch", 127 },
    { "MissingImageChannel", 128 },
    { "NegativeOrZeroImageSize", 129 },
    { "NonOS2HeaderSizeError", 130 },
    { "NotEnoughTiles", 131 },
    { "StaticPlanesValueNotEqualToOne", 132 },
    { "SubsamplingRequiresEvenWidth", 133 },
    { "TooMuchImageDataInFile", 134 },
    { "UnableToReadColormapFromDumpFile", 135 },
    { "UnableToReadColorProfile", 136 },
    { "UnableToReadExtensionBlock", 137 },
    { "UnableToReadGenericProfile", 138 },
    { "UnableToReadImageData", 139 },
    { "UnableToReadImageHeader", 140 },
    { "UnableToReadIPTCProfile", 141 },
    { "UnableToReadPixmapFromDumpFile", 142 },
    { "UnableToReadSubImageData", 143 },
    { "UnableToReadVIDImage", 144 },
    { "UnableToReadWindowNameFromDumpFile", 145 },
    { "UnableToRunlengthDecodeImage", 146 },
    { "UnableToUncompressImage", 147 },
    { "UnexpectedEndOfFile", 148 },
    { "UnexpectedSamplingFactor", 149 },
    { "UnknownPatternType", 150 },
    { "UnrecognizedBitsPerPixel", 151 },
    { "UnrecognizedImageCompression", 152 },
    { "UnrecognizedNumberOfColors", 153 },
    { "UnrecognizedXWDHeader", 154 },
    { "UnableToPersistKey", 155 },
    { "CompressionNotValid", 156 },
    { "ImproperImageHeader", 157 },
    { "LengthAndFilesizeDoNotMatch", 158 },
    { "NegativeOrZeroImageSize", 159 },
    { "NonOS2HeaderSizeError", 160 },
    { "SkipToSyncByte", 161 },
    { "StaticPlanesValueNotEqualToOne", 162 },
    { "UnrecognizedBitsPerPixel", 163 },
    { "UnrecognizedImageCompression", 164 },
    { "DelegateFailed", 165 },
    { "FailedToAllocateArgumentList", 166 },
    { "FailedToAllocateGhostscriptInterpreter", 167 },
    { "FailedToComputeOutputSize", 168 },
    { "FailedToFindGhostscript", 169 },
    { "FailedToRenderFile", 170 },
    { "FailedToScanFile", 171 },
    { "NoTagFound", 172 },
    { "PostscriptDelegateFailed", 173 },
    { "UnableToCreateImage", 174 },
    { "UnableToCreateImageComponent", 175 },
    { "UnableToDecodeImageFile", 176 },
    { "UnableToEncodeImageFile", 177 },
    { "UnableToInitializeFPXLibrary", 178 },
    { "UnableToInitializeWMFLibrary", 179 },
    { "UnableToManageJP2Stream", 180 },
    { "UnableToWriteSVGFormat", 181 },
    { "WebPABIMismatch", 182 },
    { "Default", 183 },
    { "Default", 184 },
    { "AlreadyPushingPatternDefinition", 185 },
    { "NonconformingDrawingPrimitiveDefinition", 186 },
    { "TooManyCoordinates", 187 },
    { "UnableToPrint", 188 },
    { "UnbalancedGraphicContextPushPop", 189 },
    { "Default", 190 },
    { "NotARelativeURL", 191 },
    { "NotCurrentlyPushingPatternDefinition", 192 },
    { "URLNotFound", 193 },
    { "UnableToCreateTemporaryFile", 194 },
    { "UnableToOpenFile", 195 },
    { "UnableToWriteFile", 196 },
    { "Default", 197 },
    { "Default", 198 },
    { "AngleIsDiscontinuous", 199 },
    { "ColorspaceColorProfileMismatch", 200 },
    { "ImageColorspaceDiffers", 201 },
    { "ImageColorspaceMismatch", 202 },
    { "ImageDifferenceExceedsLimit", 203 },
    { "ImageDoesNotContainResolution", 204 },
    { "ImageOpacityDiffers", 205 },
    { "ImageSequenceIsRequired", 206 },
    { "ImageSizeDiffers", 207 },
    { "InvalidColormapIndex", 208 },
    { "LeftAndRightImageSizesDiffer", 209 },
    { "NoImagesWereFound", 210 },
    { "NoImagesWereLoaded", 211 },
    { "NoLocaleImageAttribute", 212 },
    { "TooManyClusters", 213 },
    { "UnableToAppendImage", 214 },
    { "UnableToAssignProfile", 215 },
    { "UnableToAverageImage", 216 },
    { "UnableToCoalesceImage", 217 },
    { "UnableToCompareImages", 218 },
    { "UnableToCreateImageMosaic", 219 },
    { "UnableToCreateStereoImage", 220 },
    { "UnableToDeconstructImageSequence", 221 },
    { "UnableToFlattenImage", 222 },
    { "UnableToGetClipMask", 223 },
    { "UnableToHandleImageChannel", 224 },
    { "UnableToResizeImage", 225 },
    { "UnableToSegmentImage", 226 },
    { "UnableToSetClipMask", 227 },
    { "UnableToShearImage", 228 },
    { "WidthOrHeightExceedsLimit", 229 },
    { "UnableToPersistKey", 230 },
    { "Default", 231 },
    { "DPSLibraryIsNotAvailable", 232 },
    { "FPXLibraryIsNotAvailable", 233 },
    { "FreeTypeLibraryIsNotAvailable", 234 },
    { "JPEGLibraryIsNotAvailable", 235 },
    { "LCMSLibraryIsNotAvailable", 236 },
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
    { "Default", 253 },
    { "Default", 254 },
    { "UserRequestedTerminationBySignal", 255 },
    { "Default", 256 },
    { "BevelWidthIsNegative", 257 },
    { "ColorSeparatedImageRequired", 258 },
    { "FrameIsLessThanImageSize", 259 },
    { "GeometryDimensionsAreZero", 260 },
    { "GeometryDoesNotContainImage", 261 },
    { "HaldClutImageDimensionsInvalid", 262 },
    { "ImagesAreNotTheSameSize", 263 },
    { "ImageSizeMustExceedBevelWidth", 264 },
    { "ImageSmallerThanKernelWidth", 265 },
    { "ImageSmallerThanRadius", 266 },
    { "ImageWidthsOrHeightsDiffer", 267 },
    { "InputImagesAlreadySpecified", 268 },
    { "InvalidSubimageSpecification", 269 },
    { "KernelRadiusIsTooSmall", 270 },
    { "KernelWidthMustBeAnOddNumber", 271 },
    { "MatrixIsNotSquare", 272 },
    { "MatrixOrderOutOfRange", 273 },
    { "MissingAnImageFilename", 274 },
    { "MissingArgument", 275 },
    { "MustSpecifyAnImageName", 276 },
    { "MustSpecifyImageSize", 277 },
    { "NoBlobDefined", 278 },
    { "NoImagesDefined", 279 },
    { "NonzeroWidthAndHeightRequired", 280 },
    { "NoProfileNameWasGiven", 281 },
    { "NullBlobArgument", 282 },
    { "ReferenceImageRequired", 283 },
    { "ReferenceIsNotMyType", 284 },
    { "RegionAreaExceedsLimit", 285 },
    { "RequestDidNotReturnAnImage", 286 },
    { "SteganoImageRequired", 287 },
    { "StereoImageRequired", 288 },
    { "SubimageSpecificationReturnsNoImages", 289 },
    { "UnableToAddOrRemoveProfile", 290 },
    { "UnableToAverageImageSequence", 291 },
    { "UnableToBlurImage", 292 },
    { "UnableToChopImage", 293 },
    { "UnableToColorMatrixImage", 294 },
    { "UnableToConstituteImage", 295 },
    { "UnableToConvolveImage", 296 },
    { "UnableToEdgeImage", 297 },
    { "UnableToEqualizeImage", 298 },
    { "UnableToFilterImage", 299 },
    { "UnableToFormatImageMetadata", 300 },
    { "UnableToFrameImage", 301 },
    { "UnableToOilPaintImage", 302 },
    { "UnableToPaintImage", 303 },
    { "UnableToRaiseImage", 304 },
    { "UnableToSharpenImage", 305 },
    { "UnableToThresholdImage", 306 },
    { "UnableToWaveImage", 307 },
    { "UnrecognizedAttribute", 308 },
    { "UnrecognizedChannelType", 309 },
    { "UnrecognizedColor", 310 },
    { "UnrecognizedColormapType", 311 },
    { "UnrecognizedColorspace", 312 },
    { "UnrecognizedCommand", 313 },
    { "UnrecognizedComposeOperator", 314 },
    { "UnrecognizedDisposeMethod", 315 },
    { "UnrecognizedElement", 316 },
    { "UnrecognizedEndianType", 317 },
    { "UnrecognizedGravityType", 318 },
    { "UnrecognizedHighlightStyle", 319 },
    { "UnrecognizedImageCompression", 320 },
    { "UnrecognizedImageFilter", 321 },
    { "UnrecognizedImageFormat", 322 },
    { "UnrecognizedImageMode", 323 },
    { "UnrecognizedImageType", 324 },
    { "UnrecognizedIntentType", 325 },
    { "UnrecognizedInterlaceType", 326 },
    { "UnrecognizedListType", 327 },
    { "UnrecognizedMetric", 328 },
    { "UnrecognizedModeType", 329 },
    { "UnrecognizedNoiseType", 330 },
    { "UnrecognizedOperator", 331 },
    { "UnrecognizedOption", 332 },
    { "UnrecognizedPerlMagickMethod", 333 },
    { "UnrecognizedPixelMap", 334 },
    { "UnrecognizedPreviewType", 335 },
    { "UnrecognizedResourceType", 336 },
    { "UnrecognizedType", 337 },
    { "UnrecognizedUnitsType", 338 },
    { "UnrecognizedVirtualPixelMethod", 339 },
    { "UnsupportedSamplingFactor", 340 },
    { "UsageError", 341 },
    { "InvalidColorspaceType", 342 },
    { "InvalidEndianType", 343 },
    { "InvalidImageType", 344 },
    { "InvalidInterlaceType", 345 },
    { "MissingAnImageFilename", 346 },
    { "MissingArgument", 347 },
    { "NoImagesWereLoaded", 348 },
    { "OptionLengthExceedsLimit", 349 },
    { "RequestDidNotReturnAnImage", 350 },
    { "UnableToOpenXServer", 351 },
    { "UnableToPersistKey", 352 },
    { "UnrecognizedColormapType", 353 },
    { "UnrecognizedColorspaceType", 354 },
    { "UnrecognizedDisposeMethod", 355 },
    { "UnrecognizedEndianType", 356 },
    { "UnrecognizedFilterType", 357 },
    { "UnrecognizedImageCompressionType", 358 },
    { "UnrecognizedImageType", 359 },
    { "UnrecognizedInterlaceType", 360 },
    { "UnrecognizedOption", 361 },
    { "UnrecognizedResourceType", 362 },
    { "UnrecognizedVirtualPixelMethod", 363 },
    { "UnrecognizedColor", 364 },
    { "ImageExpected", 365 },
    { "ImageInfoExpected", 366 },
    { "StructureSizeMismatch", 367 },
    { "UnableToGetRegistryID", 368 },
    { "UnableToLocateImage", 369 },
    { "UnableToSetRegistry", 370 },
    { "Default", 371 },
    { "Default", 372 },
    { "CacheResourcesExhausted", 373 },
    { "ImagePixelHeightLimitExceeded", 374 },
    { "ImagePixelLimitExceeded", 375 },
    { "ImagePixelWidthLimitExceeded", 376 },
    { "MemoryAllocationFailed", 377 },
    { "NoPixelsDefinedInCache", 378 },
    { "PixelCacheAllocationFailed", 379 },
    { "UnableToAddColorProfile", 380 },
    { "UnableToAddGenericProfile", 381 },
    { "UnableToAddIPTCProfile", 382 },
    { "UnableToAllocateCoefficients", 383 },
    { "UnableToAllocateColormap", 384 },
    { "UnableToAllocateICCProfile", 385 },
    { "UnableToAllocateString", 386 },
    { "UnableToAnnotateImage", 387 },
    { "UnableToAverageImageSequence", 388 },
    { "UnableToCloneDrawingWand", 389 },
    { "UnableToCloneImage", 390 },
    { "UnableToComputeImageSignature", 391 },
    { "UnableToConstituteImage", 392 },
    { "UnableToConvertFont", 393 },
    { "UnableToConvertStringToTokens", 394 },
    { "UnableToCreateColormap", 395 },
    { "UnableToCreateColorTransform", 396 },
    { "UnableToCreateCommandWidget", 397 },
    { "UnableToCreateImageGroup", 398 },
    { "UnableToCreateImageMontage", 399 },
    { "UnableToCreateXWindow", 400 },
    { "UnableToCropImage", 401 },
    { "UnableToDespeckleImage", 402 },
    { "UnableToDetermineImageClass", 403 },
    { "UnableToDetermineTheNumberOfImageColors", 404 },
    { "UnableToDitherImage", 405 },
    { "UnableToDrawOnImage", 406 },
    { "UnableToEdgeImage", 407 },
    { "UnableToEmbossImage", 408 },
    { "UnableToEnhanceImage", 409 },
    { "UnableToFloodfillImage", 410 },
    { "UnableToGammaCorrectImage", 411 },
    { "UnableToGetBestIconSize", 412 },
    { "UnableToGetFromRegistry", 413 },
    { "UnableToGetPackageInfo", 414 },
    { "UnableToLevelImage", 415 },
    { "UnableToMagnifyImage", 416 },
    { "UnableToManageColor", 417 },
    { "UnableToMapImage", 418 },
    { "UnableToMapImageSequence", 419 },
    { "UnableToMedianFilterImage", 420 },
    { "UnableToMotionBlurImage", 421 },
    { "UnableToNoiseFilterImage", 422 },
    { "UnableToNormalizeImage", 423 },
    { "UnableToOpenColorProfile", 424 },
    { "UnableToQuantizeImage", 425 },
    { "UnableToQuantizeImageSequence", 426 },
    { "UnableToReadTextChunk", 427 },
    { "UnableToReadXImage", 428 },
    { "UnableToReadXServerColormap", 429 },
    { "UnableToResizeImage", 430 },
    { "UnableToRotateImage", 431 },
    { "UnableToSampleImage", 432 },
    { "UnableToScaleImage", 433 },
    { "UnableToSelectImage", 434 },
    { "UnableToSharpenImage", 435 },
    { "UnableToShaveImage", 436 },
    { "UnableToShearImage", 437 },
    { "UnableToSortImageColormap", 438 },
    { "UnableToThresholdImage", 439 },
    { "UnableToTransformColorspace", 440 },
    { "MemoryAllocationFailed", 441 },
    { "SemaporeOperationFailed", 442 },
    { "UnableToAllocateAscii85Info", 443 },
    { "UnableToAllocateCacheInfo", 444 },
    { "UnableToAllocateCacheView", 445 },
    { "UnableToAllocateColorInfo", 446 },
    { "UnableToAllocateDashPattern", 447 },
    { "UnableToAllocateDelegateInfo", 448 },
    { "UnableToAllocateDerivatives", 449 },
    { "UnableToAllocateDrawContext", 450 },
    { "UnableToAllocateDrawInfo", 451 },
    { "UnableToAllocateDrawingWand", 452 },
    { "UnableToAllocateGammaMap", 453 },
    { "UnableToAllocateImage", 454 },
    { "UnableToAllocateImagePixels", 455 },
    { "UnableToAllocateLogInfo", 456 },
    { "UnableToAllocateMagicInfo", 457 },
    { "UnableToAllocateMagickInfo", 458 },
    { "UnableToAllocateModuleInfo", 459 },
    { "UnableToAllocateMontageInfo", 460 },
    { "UnableToAllocateQuantizeInfo", 461 },
    { "UnableToAllocateRandomKernel", 462 },
    { "UnableToAllocateRegistryInfo", 463 },
    { "UnableToAllocateSemaphoreInfo", 464 },
    { "UnableToAllocateString", 465 },
    { "UnableToAllocateTypeInfo", 466 },
    { "UnableToAllocateWand", 467 },
    { "UnableToAnimateImageSequence", 468 },
    { "UnableToCloneBlobInfo", 469 },
    { "UnableToCloneCacheInfo", 470 },
    { "UnableToCloneImage", 471 },
    { "UnableToCloneImageInfo", 472 },
    { "UnableToConcatenateString", 473 },
    { "UnableToConvertText", 474 },
    { "UnableToCreateColormap", 475 },
    { "UnableToDestroySemaphore", 476 },
    { "UnableToDisplayImage", 477 },
    { "UnableToEscapeString", 478 },
    { "UnableToInitializeSemaphore", 479 },
    { "UnableToInterpretMSLImage", 480 },
    { "UnableToLockSemaphore", 481 },
    { "UnableToUnlockSemaphore", 482 },
    { "MemoryAllocationFailed", 483 },
    { "ImageDoesNotContainTheStreamGeometry", 484 },
    { "NoStreamHandlerIsDefined", 485 },
    { "PixelCacheIsNotOpen", 486 },
    { "UnableToAcquirePixelStream", 487 },
    { "UnableToSetPixelStream", 488 },
    { "UnableToSyncPixelStream", 489 },
    { "Default", 490 },
    { "Default", 491 },
    { "FontSubstitutionRequired", 492 },
    { "UnableToGetTypeMetrics", 493 },
    { "UnableToInitializeFreetypeLibrary", 494 },
    { "UnableToReadFont", 495 },
    { "UnrecognizedFontEncoding", 496 },
    { "Default", 497 },
    { "Default", 498 },
    { "InvalidColormapIndex", 499 },
    { "WandAPINotImplemented", 500 },
    { "WandContainsNoImageIndexs", 501 },
    { "WandContainsNoImages", 502 },
    { "ColorIsNotKnownToServer", 503 },
    { "NoWindowWithSpecifiedIDExists", 504 },
    { "StandardColormapIsNotInitialized", 505 },
    { "UnableToConnectToRemoteDisplay", 506 },
    { "UnableToCreateBitmap", 507 },
    { "UnableToCreateColormap", 508 },
    { "UnableToCreatePixmap", 509 },
    { "UnableToCreateProperty", 510 },
    { "UnableToCreateStandardColormap", 511 },
    { "UnableToDisplayImageInfo", 512 },
    { "UnableToGetProperty", 513 },
    { "UnableToGetStandardColormap", 514 },
    { "UnableToGetVisual", 515 },
    { "UnableToGrabMouse", 516 },
    { "UnableToLoadFont", 517 },
    { "UnableToMatchVisualToStandardColormap", 518 },
    { "UnableToOpenXServer", 519 },
    { "UnableToReadXAttributes", 520 },
    { "UnableToReadXWindowImage", 521 },
    { "UnrecognizedColormapType", 522 },
    { "UnrecognizedGravityType", 523 },
    { "UnrecognizedVisualSpecifier", 524 },
    { "UnableToAllocateXHints", 525 },
    { "UnableToCreateCursor", 526 },
    { "UnableToCreateGraphicContext", 527 },
    { "UnableToCreateStandardColormap", 528 },
    { "UnableToCreateTextProperty", 529 },
    { "UnableToCreateXImage", 530 },
    { "UnableToCreateXPixmap", 531 },
    { "UnableToCreateXWindow", 532 },
    { "UnableToDisplayImage", 533 },
    { "UnableToDitherImage", 534 },
    { "UnableToGetPixelInfo", 535 },
    { "UnableToGetVisual", 536 },
    { "UnableToLoadFont", 537 },
    { "UnableToMakeXWindow", 538 },
    { "UnableToOpenXServer", 539 },
    { "UnableToViewFonts", 540 },
    { "UnableToGetVisual", 541 },
    { "UsingDefaultVisual", 542 },
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
    "Unable to write to temporary file",
    "Unable to zip-compress image",
    "Unsupported cell type in the matrix",
    "WebP decoding failed: user abort",
    "WebP encoding failed: unknown reason",
    "WebP encoding failed: bad dimension",
    "WebP encoding failed: bad write",
    "WebP encoding failed: bitstream out of memory",
    "WebP encoding failed: File too big (> 4GB)",
    "WebP encoding failed: invalid configuration",
    "WebP encoding failed: null parameter",
    "WebP encoding failed: out of memory",
    "WebP encoding failed: partition 0 overflow (> 512K)",
    "WebP encoding failed: partition overflow (> 16M)",
    "WebP encoding failed: user abort",
    "Invalid WebP configuration parameters supplied",
    "WebP failed: invalid parameter",
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
    "Unable to change to working directory",
    "Unable to get current working directory",
    "Unable to restore current working directory",
    "default warning",
    "An error has occurred reading from file",
    "An error has occurred writing to file",
    "Colormap exceeded colors limit",
    "Compression not valid",
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
    "Unrecognized number of colors",
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
    "Delegate failed",
    "Failed to allocate argument list.",
    "Failed to allocate Ghostscript interpreter.",
    "Failed to compute output size",
    "Failed to find Ghostscript (not installed?).",
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
    "WebP library ABI does not match header ABI (build issue!)",
    "default error",
    "default warning",
    "Already pushing pattern definition",
    "Non-conforming drawing primitive definition",
    "too many coordinates",
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
    "Image pixel height limit exceeded (see -limit Height)",
    "Image pixel limit exceeded (see -limit Pixels)",
    "Image pixel width limit exceeded (see -limit Width)",
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
