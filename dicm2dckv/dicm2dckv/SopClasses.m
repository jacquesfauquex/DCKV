//
//  SopClasses.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-02-29.
//

#include <Foundation/Foundation.h>
#include "SopClasses.h"

const char *scstr[]={
"1.2.840.10008.1.1",//Verification
"1.2.840.10008.1.3.10",//MediaStorageDirectoryStorage
"1.2.840.10008.1.9",//BasicStudyContentNotification
"1.2.840.10008.1.20.1",//StorageCommitmentPushModel
"1.2.840.10008.1.20.2",//StorageCommitmentPullModel (Retired)
"1.2.840.10008.1.40",//ProceduralEventLogging"
"1.2.840.10008.1.42",//SubstanceAdministrationLogging
"1.2.840.10008.3.1.2.1.1",//DetachedPatientManagement (Retired)
"1.2.840.10008.3.1.2.1.4",//DetachedPatientManagementMeta (Retired)
"1.2.840.10008.3.1.2.2.1",//DetachedVisitManagement (Retired)
"1.2.840.10008.3.1.2.3.1",//DetachedStudyManagement (Retired)
"1.2.840.10008.3.1.2.3.2",//StudyComponentManagement (Retired)
"1.2.840.10008.3.1.2.3.3",//ModalityPerformedProcedureStep
"1.2.840.10008.3.1.2.3.4",//ModalityPerformedProcedureStepRetrieve
"1.2.840.10008.3.1.2.3.5",//ModalityPerformedProcedureStepNotification
"1.2.840.10008.3.1.2.5.1",//DetachedResultsManagement (Retired)
"1.2.840.10008.3.1.2.5.4",//DetachedResultsManagementMeta (Retired)
"1.2.840.10008.3.1.2.5.5",//DetachedStudyManagementMeta (Retired)
"1.2.840.10008.3.1.2.6.1",//DetachedInterpretationManagement (Retired)
"1.2.840.10008.5.1.1.1",//BasicFilmSession
"1.2.840.10008.5.1.1.2",//BasicFilmBox
"1.2.840.10008.5.1.1.4",//BasicGrayscaleImageBox
"1.2.840.10008.5.1.1.4.1",//BasicColorImageBox
"1.2.840.10008.5.1.1.4.2",//ReferencedImageBox
"1.2.840.10008.5.1.1.9",//BasicGrayscalePrintManagementMeta
"1.2.840.10008.5.1.1.9.1",//ReferencedGrayscalePrintManagementMeta
"1.2.840.10008.5.1.1.14",//PrintJob
"1.2.840.10008.5.1.1.15",//BasicAnnotationBox
"1.2.840.10008.5.1.1.16",//Printer
"1.2.840.10008.5.1.1.16.376",//PrinterConfigurationRetrieval
"1.2.840.10008.5.1.1.18",//BasicColorPrintManagementMeta
"1.2.840.10008.5.1.1.18.1",//ReferencedColorPrintManagementMeta (Retired)
"1.2.840.10008.5.1.1.22",//VOILUTBox
"1.2.840.10008.5.1.1.23",//PresentationLUT
"1.2.840.10008.5.1.1.24",//ImageOverlayBox (Retired)
"1.2.840.10008.5.1.1.24.1",//BasicPrintImageOverlayBox (Retired)
"1.2.840.10008.5.1.1.26",//PrintQueueManagement (Retired)
"1.2.840.10008.5.1.1.27",//StoredPrintStorage (Retired)
"1.2.840.10008.5.1.1.29",//HardcopyGrayscaleImageStorage (Retired)
"1.2.840.10008.5.1.1.30",//HardcopyColorImageStorage (Retired)
"1.2.840.10008.5.1.1.31",//PullPrintRequest (Retired)
"1.2.840.10008.5.1.1.32",//PullStoredPrintManagementMeta (Retired)
"1.2.840.10008.5.1.1.33",//MediaCreationManagement
"1.2.840.10008.5.1.1.40",//DisplaySystem
"1.2.840.10008.5.1.4.1.1.1",//ComputedRadiographyImageStorage
"1.2.840.10008.5.1.4.1.1.1.1",//DigitalXRayImageStorageForPresentation
"1.2.840.10008.5.1.4.1.1.1.1.1",//DigitalXRayImageStorageForProcessing
"1.2.840.10008.5.1.4.1.1.1.2",//DigitalMammographyXRayImageStorageForPresentation
"1.2.840.10008.5.1.4.1.1.1.2.1",//DigitalMammographyXRayImageStorageForProcessing
"1.2.840.10008.5.1.4.1.1.1.3",//DigitalIntraOralXRayImageStorageForPresentation
"1.2.840.10008.5.1.4.1.1.1.3.1",//DigitalIntraOralXRayImageStorageForProcessing
"1.2.840.10008.5.1.4.1.1.2",//CTImageStorage
"1.2.840.10008.5.1.4.1.1.2.1",//EnhancedCTImageStorage
"1.2.840.10008.5.1.4.1.1.2.2",//LegacyConvertedEnhancedCTImageStorage
"1.2.840.10008.5.1.4.1.1.3",//UltrasoundMultiFrameImageStorageRetired (Retired)
"1.2.840.10008.5.1.4.1.1.3.1",//UltrasoundMultiFrameImageStorage
"1.2.840.10008.5.1.4.1.1.4",//MRImageStorage
"1.2.840.10008.5.1.4.1.1.4.1",//EnhancedMRImageStorage
"1.2.840.10008.5.1.4.1.1.4.2",//MRSpectroscopyStorage
"1.2.840.10008.5.1.4.1.1.4.3",//EnhancedMRColorImageStorage
"1.2.840.10008.5.1.4.1.1.4.4",//LegacyConvertedEnhancedMRImageStorage
"1.2.840.10008.5.1.4.1.1.5",//NuclearMedicineImageStorageRetired (Retired)
"1.2.840.10008.5.1.4.1.1.6",//UltrasoundImageStorageRetired (Retired)
"1.2.840.10008.5.1.4.1.1.6.1",//UltrasoundImageStorage
"1.2.840.10008.5.1.4.1.1.6.2",//EnhancedUSVolumeStorage
"1.2.840.10008.5.1.4.1.1.6.3",//PhotoacousticImageStorage
"1.2.840.10008.5.1.4.1.1.7",//SecondaryCaptureImageStorage
"1.2.840.10008.5.1.4.1.1.7.1",//MultiFrameSingleBitSecondaryCaptureImageStorage
"1.2.840.10008.5.1.4.1.1.7.2",//MultiFrameGrayscaleByteSecondaryCaptureImageStorage
"1.2.840.10008.5.1.4.1.1.7.3",//MultiFrameGrayscaleWordSecondaryCaptureImageStorage
"1.2.840.10008.5.1.4.1.1.7.4",//MultiFrameTrueColorSecondaryCaptureImageStorage
"1.2.840.10008.5.1.4.1.1.8",//StandaloneOverlayStorage (Retired)
"1.2.840.10008.5.1.4.1.1.9",//StandaloneCurveStorage (Retired)
"1.2.840.10008.5.1.4.1.1.9.1",//WaveformStorageTrial (Retired)
"1.2.840.10008.5.1.4.1.1.9.1.1",//TwelveLeadECGWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.1.2",//GeneralECGWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.1.3",//AmbulatoryECGWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.1.4",//General32bitECGWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.2.1",//HemodynamicWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.3.1",//CardiacElectrophysiologyWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.4.1",//BasicVoiceAudioWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.4.2",//GeneralAudioWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.5.1",//ArterialPulseWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.6.1",//RespiratoryWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.6.2",//MultichannelRespiratoryWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.7.1",//RoutineScalpElectroencephalogramWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.7.2",//ElectromyogramWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.7.3",//ElectrooculogramWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.7.4",//SleepElectroencephalogramWaveformStorage
"1.2.840.10008.5.1.4.1.1.9.8.1",//BodyPositionWaveformStorage
"1.2.840.10008.5.1.4.1.1.10",//StandaloneModalityLUTStorage (Retired)
"1.2.840.10008.5.1.4.1.1.11",//StandaloneVOILUTStorage (Retired)
"1.2.840.10008.5.1.4.1.1.11.1",//GrayscaleSoftcopyPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.2",//ColorSoftcopyPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.3",//PseudoColorSoftcopyPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.4",//BlendingSoftcopyPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.5",//XAXRFGrayscaleSoftcopyPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.6",//GrayscalePlanarMPRVolumetricPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.7",//CompositingPlanarMPRVolumetricPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.8",//AdvancedBlendingPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.9",//VolumeRenderingVolumetricPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.10",//SegmentedVolumeRenderingVolumetricPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.11",//MultipleVolumeRenderingVolumetricPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.11.12",//VariableModalityLUTSoftcopyPresentationStateStorage
"1.2.840.10008.5.1.4.1.1.12.1",//XRayAngiographicImageStorage
"1.2.840.10008.5.1.4.1.1.12.1.1",//EnhancedXAImageStorage
"1.2.840.10008.5.1.4.1.1.12.2",//XRayRadiofluoroscopicImageStorage
"1.2.840.10008.5.1.4.1.1.12.2.1",//EnhancedXRFImageStorage
"1.2.840.10008.5.1.4.1.1.12.3",//XRayAngiographicBiPlaneImageStorage (Retired)
"1.2.840.10008.5.1.4.1.1.12.77",//(Retired)
"1.2.840.10008.5.1.4.1.1.13.1.1",//XRay3DAngiographicImageStorage
"1.2.840.10008.5.1.4.1.1.13.1.2",//XRay3DCraniofacialImageStorage
"1.2.840.10008.5.1.4.1.1.13.1.3",//BreastTomosynthesisImageStorage
"1.2.840.10008.5.1.4.1.1.13.1.4",//BreastProjectionXRayImageStorageForPresentation
"1.2.840.10008.5.1.4.1.1.13.1.5",//BreastProjectionXRayImageStorageForProcessing
"1.2.840.10008.5.1.4.1.1.14.1",//IntravascularOpticalCoherenceTomographyImageStorageForPresentation
"1.2.840.10008.5.1.4.1.1.14.2",//IntravascularOpticalCoherenceTomographyImageStorageForProcessing
"1.2.840.10008.5.1.4.1.1.20",//NuclearMedicineImageStorage
"1.2.840.10008.5.1.4.1.1.30",//ParametricMapStorage
"1.2.840.10008.5.1.4.1.1.40",//(Retired)
"1.2.840.10008.5.1.4.1.1.66",//RawDataStorage
"1.2.840.10008.5.1.4.1.1.66.1",//SpatialRegistrationStorage
"1.2.840.10008.5.1.4.1.1.66.2",//SpatialFiducialsStorage
"1.2.840.10008.5.1.4.1.1.66.3",//DeformableSpatialRegistrationStorage
"1.2.840.10008.5.1.4.1.1.66.4",//SegmentationStorage
"1.2.840.10008.5.1.4.1.1.66.5",//SurfaceSegmentationStorage
"1.2.840.10008.5.1.4.1.1.66.6",//TractographyResultsStorage
"1.2.840.10008.5.1.4.1.1.67",//RealWorldValueMappingStorage
"1.2.840.10008.5.1.4.1.1.68.1",//SurfaceScanMeshStorage
"1.2.840.10008.5.1.4.1.1.68.2",//SurfaceScanPointCloudStorage
"1.2.840.10008.5.1.4.1.1.77.1",//VLImageStorageTrial (Retired)
"1.2.840.10008.5.1.4.1.1.77.2",//VLMultiFrameImageStorageTrial (Retired)
"1.2.840.10008.5.1.4.1.1.77.1.1",//VLEndoscopicImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.1.1",//VideoEndoscopicImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.2",//VLMicroscopicImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.2.1",//VideoMicroscopicImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.3",//VLSlideCoordinatesMicroscopicImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.4",//VLPhotographicImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.4.1",//VideoPhotographicImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.5.1",//OphthalmicPhotography8BitImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.5.2",//OphthalmicPhotography16BitImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.5.3",//StereometricRelationshipStorage
"1.2.840.10008.5.1.4.1.1.77.1.5.4",//OphthalmicTomographyImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.5.5",//WideFieldOphthalmicPhotographyStereographicProjectionImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.5.6",//WideFieldOphthalmicPhotography3DCoordinatesImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.5.7",//OphthalmicOpticalCoherenceTomographyEnFaceImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.5.8",//OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage
"1.2.840.10008.5.1.4.1.1.77.1.6",//VLWholeSlideMicroscopyImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.7",//DermoscopicPhotographyImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.8",//ConfocalMicroscopyImageStorage
"1.2.840.10008.5.1.4.1.1.77.1.9",//ConfocalMicroscopyTiledPyramidalImageStorage
"1.2.840.10008.5.1.4.1.1.78.1",//LensometryMeasurementsStorage
"1.2.840.10008.5.1.4.1.1.78.2",//AutorefractionMeasurementsStorage
"1.2.840.10008.5.1.4.1.1.78.3",//KeratometryMeasurementsStorage
"1.2.840.10008.5.1.4.1.1.78.4",//SubjectiveRefractionMeasurementsStorage
"1.2.840.10008.5.1.4.1.1.78.5",//VisualAcuityMeasurementsStorage
"1.2.840.10008.5.1.4.1.1.78.6",//SpectaclePrescriptionReportStorage
"1.2.840.10008.5.1.4.1.1.78.7",//OphthalmicAxialMeasurementsStorage
"1.2.840.10008.5.1.4.1.1.78.8",//IntraocularLensCalculationsStorage
"1.2.840.10008.5.1.4.1.1.79.1",//MacularGridThicknessAndVolumeReportStorage
"1.2.840.10008.5.1.4.1.1.80.1",//OphthalmicVisualFieldStaticPerimetryMeasurementsStorage
"1.2.840.10008.5.1.4.1.1.81.1",//OphthalmicThicknessMapStorage
"1.2.840.10008.5.1.4.1.1.82.1",//CornealTopographyMapStorage
"1.2.840.10008.5.1.4.1.1.88.1",//TextSRStorageTrial (Retired)
"1.2.840.10008.5.1.4.1.1.88.2",//AudioSRStorageTrial (Retired)
"1.2.840.10008.5.1.4.1.1.88.3",//DetailSRStorageTrial (Retired)
"1.2.840.10008.5.1.4.1.1.88.4",//ComprehensiveSRStorageTrial (Retired)
"1.2.840.10008.5.1.4.1.1.88.11",//BasicTextSRStorage
"1.2.840.10008.5.1.4.1.1.88.22",//EnhancedSRStorage
"1.2.840.10008.5.1.4.1.1.88.33",//ComprehensiveSRStorage
"1.2.840.10008.5.1.4.1.1.88.34",//Comprehensive3DSRStorage
"1.2.840.10008.5.1.4.1.1.88.35",//ExtensibleSRStorage
"1.2.840.10008.5.1.4.1.1.88.40",//ProcedureLogStorage
"1.2.840.10008.5.1.4.1.1.88.50",//MammographyCADSRStorage
"1.2.840.10008.5.1.4.1.1.88.59",//KeyObjectSelectionDocumentStorage
"1.2.840.10008.5.1.4.1.1.88.65",//ChestCADSRStorage
"1.2.840.10008.5.1.4.1.1.88.67",//XRayRadiationDoseSRStorage
"1.2.840.10008.5.1.4.1.1.88.68",//RadiopharmaceuticalRadiationDoseSRStorage
"1.2.840.10008.5.1.4.1.1.88.69",//ColonCADSRStorage
"1.2.840.10008.5.1.4.1.1.88.70",//ImplantationPlanSRStorage
"1.2.840.10008.5.1.4.1.1.88.71",//AcquisitionContextSRStorage
"1.2.840.10008.5.1.4.1.1.88.72",//SimplifiedAdultEchoSRStorage
"1.2.840.10008.5.1.4.1.1.88.73",//PatientRadiationDoseSRStorage
"1.2.840.10008.5.1.4.1.1.88.74",//PlannedImagingAgentAdministrationSRStorage
"1.2.840.10008.5.1.4.1.1.88.75",//PerformedImagingAgentAdministrationSRStorage
"1.2.840.10008.5.1.4.1.1.88.76",//EnhancedXRayRadiationDoseSRStorage
"1.2.840.10008.5.1.4.1.1.90.1",//ContentAssessmentResultsStorage
"1.2.840.10008.5.1.4.1.1.91.1",//MicroscopyBulkSimpleAnnotationsStorage
"1.2.840.10008.5.1.4.1.1.104.1",//EncapsulatedPDFStorage
"1.2.840.10008.5.1.4.1.1.104.2",//EncapsulatedCDAStorage
"1.2.840.10008.5.1.4.1.1.104.3",//EncapsulatedSTLStorage
"1.2.840.10008.5.1.4.1.1.104.4",//EncapsulatedOBJStorage
"1.2.840.10008.5.1.4.1.1.104.5",//EncapsulatedMTLStorage
"1.2.840.10008.5.1.4.1.1.128",//PositronEmissionTomographyImageStorage
"1.2.840.10008.5.1.4.1.1.128.1",//LegacyConvertedEnhancedPETImageStorage
"1.2.840.10008.5.1.4.1.1.129",//StandalonePETCurveStorage
"1.2.840.10008.5.1.4.1.1.130",//EnhancedPETImageStorage
"1.2.840.10008.5.1.4.1.1.131",//BasicStructuredDisplayStorage
"1.2.840.10008.5.1.4.1.1.200.1",//CTDefinedProcedureProtocolStorage
"1.2.840.10008.5.1.4.1.1.200.2",//CTPerformedProcedureProtocolStorage
"1.2.840.10008.5.1.4.1.1.200.3",//ProtocolApprovalStorage
"1.2.840.10008.5.1.4.1.1.200.4",//ProtocolApprovalInformationModelFind
"1.2.840.10008.5.1.4.1.1.200.5",//ProtocolApprovalInformationModelMove
"1.2.840.10008.5.1.4.1.1.200.6",//ProtocolApprovalInformationModelGet
"1.2.840.10008.5.1.4.1.1.200.7",//XADefinedProcedureProtocolStorage
"1.2.840.10008.5.1.4.1.1.200.8",//XAPerformedProcedureProtocolStorage
"1.2.840.10008.5.1.4.1.1.201.1",//InventoryStorage
"1.2.840.10008.5.1.4.1.1.201.2",//InventoryFind
"1.2.840.10008.5.1.4.1.1.201.3",//InventoryMove
"1.2.840.10008.5.1.4.1.1.201.4",//InventoryGet
"1.2.840.10008.5.1.4.1.1.201.5",//InventoryCreation
"1.2.840.10008.5.1.4.1.1.201.6",//RepositoryQuery
"1.2.840.10008.5.1.4.1.1.481.1",//RTImageStorage
"1.2.840.10008.5.1.4.1.1.481.2",//RTDoseStorage
"1.2.840.10008.5.1.4.1.1.481.3",//RTStructureSetStorage
"1.2.840.10008.5.1.4.1.1.481.4",//RTBeamsTreatmentRecordStorage
"1.2.840.10008.5.1.4.1.1.481.5",//RTPlanStorage
"1.2.840.10008.5.1.4.1.1.481.6",//RTBrachyTreatmentRecordStorage
"1.2.840.10008.5.1.4.1.1.481.7",//RTTreatmentSummaryRecordStorage
"1.2.840.10008.5.1.4.1.1.481.8",//RTIonPlanStorage
"1.2.840.10008.5.1.4.1.1.481.9",//RTIonBeamsTreatmentRecordStorage
"1.2.840.10008.5.1.4.1.1.481.10",//RTPhysicianIntentStorage
"1.2.840.10008.5.1.4.1.1.481.11",//RTSegmentAnnotationStorage
"1.2.840.10008.5.1.4.1.1.481.12",//RTRadiationSetStorage
"1.2.840.10008.5.1.4.1.1.481.13",//CArmPhotonElectronRadiationStorage
"1.2.840.10008.5.1.4.1.1.481.14",//TomotherapeuticRadiationStorage
"1.2.840.10008.5.1.4.1.1.481.15",//RoboticArmRadiationStorage
"1.2.840.10008.5.1.4.1.1.481.16",//RTRadiationRecordSetStorage
"1.2.840.10008.5.1.4.1.1.481.17",//RTRadiationSalvageRecordStorage
"1.2.840.10008.5.1.4.1.1.481.18",//TomotherapeuticRadiationRecordStorage
"1.2.840.10008.5.1.4.1.1.481.19",//CArmPhotonElectronRadiationRecordStorage
"1.2.840.10008.5.1.4.1.1.481.20",//RoboticRadiationRecordStorage
"1.2.840.10008.5.1.4.1.1.481.21",//RTRadiationSetDeliveryInstructionStorage
"1.2.840.10008.5.1.4.1.1.481.22",//RTTreatmentPreparationStorage
"1.2.840.10008.5.1.4.1.1.481.23",//EnhancedRTImageStorage
"1.2.840.10008.5.1.4.1.1.481.24",//EnhancedContinuousRTImageStorage
"1.2.840.10008.5.1.4.1.1.481.25",//RTPatientPositionAcquisitionInstructionStorage
"1.2.840.10008.5.1.4.1.1.501.1",//DICOSCTImageStorage
"1.2.840.10008.5.1.4.1.1.501.2.1",//DICOSDigitalXRayImageStorageForPresentation
"1.2.840.10008.5.1.4.1.1.501.2.2",//DICOSDigitalXRayImageStorageForProcessing
"1.2.840.10008.5.1.4.1.1.501.3",//DICOSThreatDetectionReportStorage
"1.2.840.10008.5.1.4.1.1.501.4",//DICOS2DAITStorage
"1.2.840.10008.5.1.4.1.1.501.5",//DICOS3DAITStorage
"1.2.840.10008.5.1.4.1.1.501.6",//DICOSQuadrupoleResonanceStorage
"1.2.840.10008.5.1.4.1.1.601.1",//EddyCurrentImageStorage
"1.2.840.10008.5.1.4.1.1.601.2",//EddyCurrentMultiFrameImageStorage
"1.2.840.10008.5.1.4.1.2.1.1",//PatientRootQueryRetrieveInformationModelFind
"1.2.840.10008.5.1.4.1.2.1.2",//PatientRootQueryRetrieveInformationModelMove
"1.2.840.10008.5.1.4.1.2.1.3",//PatientRootQueryRetrieveInformationModelGet
"1.2.840.10008.5.1.4.1.2.2.1",//StudyRootQueryRetrieveInformationModelFind
"1.2.840.10008.5.1.4.1.2.2.2",//StudyRootQueryRetrieveInformationModelMove
"1.2.840.10008.5.1.4.1.2.2.3",//StudyRootQueryRetrieveInformationModelGet
"1.2.840.10008.5.1.4.1.2.3.1",//PatientStudyOnlyQueryRetrieveInformationModelFind
"1.2.840.10008.5.1.4.1.2.3.2",//PatientStudyOnlyQueryRetrieveInformationModelMove
"1.2.840.10008.5.1.4.1.2.3.3",//PatientStudyOnlyQueryRetrieveInformationModelGet
"1.2.840.10008.5.1.4.1.2.4.2",//CompositeInstanceRootRetrieveMove
"1.2.840.10008.5.1.4.1.2.4.3",//CompositeInstanceRootRetrieveGet
"1.2.840.10008.5.1.4.1.2.5.3",//CompositeInstanceRetrieveWithoutBulkDataGet
"1.2.840.10008.5.1.4.20.1",//DefinedProcedureProtocolInformationModelFind
"1.2.840.10008.5.1.4.20.2",//DefinedProcedureProtocolInformationModelMove
"1.2.840.10008.5.1.4.20.3",//DefinedProcedureProtocolInformationModelGet
"1.2.840.10008.5.1.4.31",//ModalityWorklistInformationModelFind
"1.2.840.10008.5.1.4.32",//GeneralPurposeWorklistManagementMeta
"1.2.840.10008.5.1.4.32.1",//GeneralPurposeWorklistInformationModelFind
"1.2.840.10008.5.1.4.32.2",//GeneralPurposeScheduledProcedureStep
"1.2.840.10008.5.1.4.32.3",//GeneralPurposePerformedProcedureStep
"1.2.840.10008.5.1.4.33",//InstanceAvailabilityNotification
"1.2.840.10008.5.1.4.34.1",//RTBeamsDeliveryInstructionStorageTrial
"1.2.840.10008.5.1.4.34.2",//RTConventionalMachineVerificationTrial
"1.2.840.10008.5.1.4.34.3",//RTIonMachineVerificationTrial (Retired)
"1.2.840.10008.5.1.4.34.4",//UnifiedWorklistAndProcedureStepTrial (Retired)
"1.2.840.10008.5.1.4.34.4.1",//UnifiedProcedureStepPushTrial (Retired)
"1.2.840.10008.5.1.4.34.4.2",//UnifiedProcedureStepWatchTrial (Retired)
"1.2.840.10008.5.1.4.34.4.3",//UnifiedProcedureStepPullTrial (Retired)
"1.2.840.10008.5.1.4.34.4.4",//UnifiedProcedureStepEventTrial
"1.2.840.10008.5.1.4.34.5",//UPSGlobalSubscriptionInstance
"1.2.840.10008.5.1.4.34.5.1",//UPSFilteredGlobalSubscriptionInstance
"1.2.840.10008.5.1.4.34.6",//UnifiedWorklistAndProcedureStep
"1.2.840.10008.5.1.4.34.6.1",//UnifiedProcedureStepPush
"1.2.840.10008.5.1.4.34.6.2",//UnifiedProcedureStepWatch
"1.2.840.10008.5.1.4.34.6.3",//UnifiedProcedureStepPull
"1.2.840.10008.5.1.4.34.6.4",//UnifiedProcedureStepEvent
"1.2.840.10008.5.1.4.34.6.5",//UnifiedProcedureStepQuery
"1.2.840.10008.5.1.4.34.7",//RTBeamsDeliveryInstructionStorage
"1.2.840.10008.5.1.4.34.8",//RTConventionalMachineVerification
"1.2.840.10008.5.1.4.34.9",//RTIonMachineVerification
"1.2.840.10008.5.1.4.34.10",//RTBrachyApplicationSetupDeliveryInstructionStorage
"1.2.840.10008.5.1.4.37.1",//GeneralRelevantPatientInformationQuery
"1.2.840.10008.5.1.4.37.2",//BreastImagingRelevantPatientInformationQuery
"1.2.840.10008.5.1.4.37.3",//CardiacRelevantPatientInformationQuery
"1.2.840.10008.5.1.4.38.1",//HangingProtocolStorage
"1.2.840.10008.5.1.4.38.2",//HangingProtocolInformationModelFind
"1.2.840.10008.5.1.4.38.3",//HangingProtocolInformationModelMove
"1.2.840.10008.5.1.4.38.4",//HangingProtocolInformationModelGet
"1.2.840.10008.5.1.4.39.1",//ColorPaletteStorage
"1.2.840.10008.5.1.4.39.2",//ColorPaletteQueryRetrieveInformationModelFind
"1.2.840.10008.5.1.4.39.3",//ColorPaletteQueryRetrieveInformationModelMove
"1.2.840.10008.5.1.4.39.4",//ColorPaletteQueryRetrieveInformationModelGet
"1.2.840.10008.5.1.4.41",//ProductCharacteristicsQuery
"1.2.840.10008.5.1.4.42",//SubstanceApprovalQuery
"1.2.840.10008.5.1.4.43.1",//GenericImplantTemplateStorage
"1.2.840.10008.5.1.4.43.2",//GenericImplantTemplateInformationModelFind
"1.2.840.10008.5.1.4.43.3",//GenericImplantTemplateInformationModelMove
"1.2.840.10008.5.1.4.43.4",//GenericImplantTemplateInformationModelGet
"1.2.840.10008.5.1.4.44.1",//ImplantAssemblyTemplateStorage
"1.2.840.10008.5.1.4.44.2",//ImplantAssemblyTemplateInformationModelFind
"1.2.840.10008.5.1.4.44.3",//ImplantAssemblyTemplateInformationModelMove
"1.2.840.10008.5.1.4.44.4",//ImplantAssemblyTemplateInformationModelGet
"1.2.840.10008.5.1.4.45.1",//ImplantTemplateGroupStorage
"1.2.840.10008.5.1.4.45.2",//ImplantTemplateGroupInformationModelFind
"1.2.840.10008.5.1.4.45.3",//ImplantTemplateGroupInformationModelMove
"1.2.840.10008.5.1.4.45.4",//ImplantTemplateGroupInformationModelGet
"1.2.840.10008.10.1",//VideoEndoscopicImageRealTimeCommunication
"1.2.840.10008.10.2",//VideoPhotographicImageRealTimeCommunication
"1.2.840.10008.10.3",//AudioWaveformRealTimeCommunication
"1.2.840.10008.10.4"//RenditionSelectionDocumentRealTimeCommunication
};

uint16 scidx( uint8_t *valbytes, uint16 vallength )
{
   uint16 idx=0x0;//verification / error
   switch (vallength-(valbytes[vallength-1]==0)){
         
      case 17: {
         switch (valbytes[16]){
            case 0x31: idx=sc_1_1;break;//Verification
            case 0x39: idx=sc_1_9;break;//BasicStudyContentNotification
         }};break;
         
      case 18: {
         switch ((valbytes[17]<<8) | valbytes[16]) {
            case 0x3034: idx=sc_1_40;break;//ProceduralEventLogging"
            case 0x3234: idx=sc_1_42;break;//SubstanceAdministrationLogging
            case 0x312E: idx=sc_10_1;break;//VideoEndoscopicImageRealTimeCommunication
            case 0x322E: idx=sc_10_2;break;//VideoPhotographicImageRealTimeCommunication
            case 0x332E: idx=sc_10_3;break;//AudioWaveformRealTimeCommunication
            case 0x342E: idx=sc_10_4;break;//RenditionSelectionDocumentRealTimeCommunication
         }};break;
         
      case 20:{
         switch (valbytes[19]){
            case 0x30: idx=sc_1_3_10;break;//MediaStorageDirectoryStorage
            case 0x31: idx=sc_1_20_1;break;//StorageCommitmentPushModel
            case 0x32: idx=sc_1_20_2;break;//StorageCommitmentPullModel (Retired)
         }};break;
         
      case 21:{
         switch (valbytes[20]){
            case 0x31: idx=sc_5_1_1_1;break;//BasicFilmSession
            case 0x32: idx=sc_5_1_1_2;break;//BasicFilmBox
            case 0x34: idx=sc_5_1_1_4;break;//BasicGrayscaleImageBox
            case 0x39: idx=sc_5_1_1_9;break;//BasicGrayscalePrintManagementMeta
        }};break;
         
      case 22:{
         switch ((valbytes[21] << 24) | (valbytes[20]<<16) | (valbytes[19]<<8) | valbytes[18]) {
            case 0x34312E31: idx=sc_5_1_1_14;break;//PrintJob
            case 0x35312E31: idx=sc_5_1_1_15;break;//BasicAnnotationBox
            case 0x36312E31: idx=sc_5_1_1_16;break;//Printer
            case 0x38312E31: idx=sc_5_1_1_18;break;//BasicColorPrintManagementMeta
            case 0x32322E31: idx=sc_5_1_1_22;break;//VOILUTBox
            case 0x33322E31: idx=sc_5_1_1_23;break;//PresentationLUT
            case 0x34322E31: idx=sc_5_1_1_24;break;//ImageOverlayBox (Retired)
            case 0x36322E31: idx=sc_5_1_1_26;break;//PrintQueueManagement (Retired)
            case 0x37322E31: idx=sc_5_1_1_27;break;//StoredPrintStorage (Retired)
            case 0x39322E31: idx=sc_5_1_1_29;break;//HardcopyGrayscaleImageStorage (Retired)
            case 0x30332E31: idx=sc_5_1_1_30;break;//HardcopyColorImageStorage (Retired)
            case 0x31332E31: idx=sc_5_1_1_31;break;//PullPrintRequest (Retired)
            case 0x32332E31: idx=sc_5_1_1_32;break;//PullStoredPrintManagementMeta (Retired)
            case 0x33332E31: idx=sc_5_1_1_33;break;//MediaCreationManagement
            case 0x30342E31: idx=sc_5_1_1_40;break;//DisplaySystem
            case 0x31332E34: idx=sc_5_1_4_31;break;//ModalityWorklistInformationModelFind
            case 0x32332E34: idx=sc_5_1_4_32;break;//GeneralPurposeWorklistManagementMeta
            case 0x33332E34: idx=sc_5_1_4_33;break;//InstanceAvailabilityNotification
            case 0x31342E34: idx=sc_5_1_4_41;break;//ProductCharacteristicsQuery
            case 0x32342E34: idx=sc_5_1_4_42;break;//SubstanceApprovalQuery
         }};break;
         
      case 23:{
         switch ((valbytes[22] << 24) | (valbytes[21]<<16) | (valbytes[20]<<8) | valbytes[19]) {
            case 0x312E312E: idx=sc_3_1_2_1_1;break;//DetachedPatientManagement (Retired)
            case 0x342E312E: idx=sc_3_1_2_1_4;break;//DetachedPatientManagementMeta (Retired)
            case 0x312E322E: idx=sc_3_1_2_2_1;break;//DetachedVisitManagement (Retired)
            case 0x312E332E: idx=sc_3_1_2_3_1;break;//DetachedStudyManagement (Retired)
            case 0x322E332E: idx=sc_3_1_2_3_2;break;//StudyComponentManagement (Retired)
            case 0x332E332E: idx=sc_3_1_2_3_3;break;//ModalityPerformedProcedureStep
            case 0x342E332E: idx=sc_3_1_2_3_4;break;//ModalityPerformedProcedureStepRetrieve
            case 0x352E332E: idx=sc_3_1_2_3_5;break;//ModalityPerformedProcedureStepNotification
            case 0x312E352E: idx=sc_3_1_2_5_1;break;//DetachedResultsManagement (Retired)
            case 0x342E352E: idx=sc_3_1_2_5_4;break;//DetachedResultsManagementMeta (Retired)
            case 0x352E352E: idx=sc_3_1_2_5_5;break;//DetachedStudyManagementMeta (Retired)
            case 0x312E362E: idx=sc_3_1_2_6_1;break;//DetachedInterpretationManagement (Retired)
            case 0x312E342E: idx=sc_5_1_1_4_1;break;//BasicColorImageBox
            case 0x322E342E: idx=sc_5_1_1_4_2;break;//ReferencedImageBox
            case 0x312E392E: idx=sc_5_1_1_9_1;break;//ReferencedGrayscalePrintManagementMeta
         }};break;
         
      case 24:{
         switch ((valbytes[23] << 24) | (valbytes[22]<<16) | (valbytes[21]<<8) | valbytes[20]) {
            case 0x312E3831: idx=sc_5_1_1_18_1;break;//ReferencedColorPrintManagementMeta (Retired)
            case 0x312E3432: idx=sc_5_1_1_24_1;break;//BasicPrintImageOverlayBox (Retired)
            case 0x312E3032: idx=sc_5_1_4_20_1;break;//DefinedProcedureProtocolInformationModelFind
            case 0x322E3032: idx=sc_5_1_4_20_2;break;//DefinedProcedureProtocolInformationModelMove
            case 0x332E3032: idx=sc_5_1_4_20_3;break;//DefinedProcedureProtocolInformationModelGet
            case 0x312E3233: idx=sc_5_1_4_32_1;break;//GeneralPurposeWorklistInformationModelFind
            case 0x322E3233: idx=sc_5_1_4_32_2;break;//GeneralPurposeScheduledProcedureStep
            case 0x332E3233: idx=sc_5_1_4_32_3;break;//GeneralPurposePerformedProcedureStep
            case 0x312E3433: idx=sc_5_1_4_34_1;break;//RTBeamsDeliveryInstructionStorageTrial
            case 0x322E3433: idx=sc_5_1_4_34_2;break;//RTConventionalMachineVerificationTrial
            case 0x332E3433: idx=sc_5_1_4_34_3;break;//RTIonMachineVerificationTrial (Retired)
            case 0x342E3433: idx=sc_5_1_4_34_4;break;//UnifiedWorklistAndProcedureStepTrial (Retired)
            case 0x352E3433: idx=sc_5_1_4_34_5;break;//UPSGlobalSubscriptionInstance
            case 0x362E3433: idx=sc_5_1_4_34_6;break;//UnifiedWorklistAndProcedureStep
            case 0x372E3433: idx=sc_5_1_4_34_7;break;//RTBeamsDeliveryInstructionStorage
            case 0x382E3433: idx=sc_5_1_4_34_8;break;//RTConventionalMachineVerification
            case 0x392E3433: idx=sc_5_1_4_34_9;break;//RTIonMachineVerification
            case 0x312E3733: idx=sc_5_1_4_37_1;break;//GeneralRelevantPatientInformationQuery
            case 0x322E3733: idx=sc_5_1_4_37_2;break;//BreastImagingRelevantPatientInformationQuery
            case 0x332E3733: idx=sc_5_1_4_37_3;break;//CardiacRelevantPatientInformationQuery
            case 0x312E3833: idx=sc_5_1_4_38_1;break;//HangingProtocolStorage
            case 0x322E3833: idx=sc_5_1_4_38_2;break;//HangingProtocolInformationModelFind
            case 0x332E3833: idx=sc_5_1_4_38_3;break;//HangingProtocolInformationModelMove
            case 0x342E3833: idx=sc_5_1_4_38_4;break;//HangingProtocolInformationModelGet
            case 0x312E3933: idx=sc_5_1_4_39_1;break;//ColorPaletteStorage
            case 0x322E3933: idx=sc_5_1_4_39_2;break;//ColorPaletteQueryRetrieveInformationModelFind
            case 0x332E3933: idx=sc_5_1_4_39_3;break;//ColorPaletteQueryRetrieveInformationModelMove
            case 0x342E3933: idx=sc_5_1_4_39_4;break;//ColorPaletteQueryRetrieveInformationModelGet
            case 0x312E3334: idx=sc_5_1_4_43_1;break;//GenericImplantTemplateStorage
            case 0x322E3334: idx=sc_5_1_4_43_2;break;//GenericImplantTemplateInformationModelFind
            case 0x332E3334: idx=sc_5_1_4_43_3;break;//GenericImplantTemplateInformationModelMove
            case 0x342E3334: idx=sc_5_1_4_43_4;break;//GenericImplantTemplateInformationModelGet
            case 0x312E3434: idx=sc_5_1_4_44_1;break;//ImplantAssemblyTemplateStorage
            case 0x322E3434: idx=sc_5_1_4_44_2;break;//ImplantAssemblyTemplateInformationModelFind
            case 0x332E3434: idx=sc_5_1_4_44_3;break;//ImplantAssemblyTemplateInformationModelMove
            case 0x342E3434: idx=sc_5_1_4_44_4;break;//ImplantAssemblyTemplateInformationModelGet
            case 0x312E3534: idx=sc_5_1_4_45_1;break;//ImplantTemplateGroupStorage
            case 0x322E3534: idx=sc_5_1_4_45_2;break;//ImplantTemplateGroupInformationModelFind
            case 0x332E3534: idx=sc_5_1_4_45_3;break;//ImplantTemplateGroupInformationModelMove
            case 0x342E3534: idx=sc_5_1_4_45_4;break;//ImplantTemplateGroupInformationModelGet
         }};break;

      case 25:{
         switch (valbytes[24]){
            case 0x31: idx=sc_5_1_4_1_1_1;break;//ComputedRadiographyImageStorage
            case 0x32: idx=sc_5_1_4_1_1_2;break;//CTImageStorage
            case 0x33: idx=sc_5_1_4_1_1_3;break;//UltrasoundMultiFrameImageStorageRetired (Retired)
            case 0x34: idx=sc_5_1_4_1_1_4;break;//MRImageStorage
            case 0x35: idx=sc_5_1_4_1_1_5;break;//NuclearMedicineImageStorageRetired (Retired)
            case 0x36: idx=sc_5_1_4_1_1_6;break;//UltrasoundImageStorageRetired (Retired)
            case 0x37: idx=sc_5_1_4_1_1_7;break;//SecondaryCaptureImageStorage
            case 0x38: idx=sc_5_1_4_1_1_8;break;//StandaloneOverlayStorage (Retired)
            case 0x39: idx=sc_5_1_4_1_1_9;break;//StandaloneCurveStorage (Retired)
            case 0x30: idx=sc_5_1_4_34_10;break;//RTBrachyApplicationSetupDeliveryInstructionStorage
         }};break;

      case 26:{
         switch ((valbytes[25] << 24) | (valbytes[24]<<16) | (valbytes[23]<<8) | valbytes[22]) {
            case 0x3637332E: idx=sc_5_1_1_16_376;break;//PrinterConfigurationRetrieval
            case 0x30312E31: idx=sc_5_1_4_1_1_10;break;//StandaloneModalityLUTStorage (Retired)
            case 0x31312E31: idx=sc_5_1_4_1_1_11;break;//StandaloneVOILUTStorage (Retired)
            case 0x30322E31: idx=sc_5_1_4_1_1_20;break;//NuclearMedicineImageStorage
            case 0x30332E31: idx=sc_5_1_4_1_1_30;break;//ParametricMapStorage
            case 0x30342E31: idx=sc_5_1_4_1_1_40;break;//(Retired)
            case 0x36362E31: idx=sc_5_1_4_1_1_66;break;//RawDataStorage
            case 0x37362E31: idx=sc_5_1_4_1_1_67;break;//RealWorldValueMappingStorage
            case 0x312E342E: idx=sc_5_1_4_34_4_1;break;//UnifiedProcedureStepPushTrial (Retired)
            case 0x322E342E: idx=sc_5_1_4_34_4_2;break;//UnifiedProcedureStepWatchTrial (Retired)
            case 0x332E342E: idx=sc_5_1_4_34_4_3;break;//UnifiedProcedureStepPullTrial (Retired)
            case 0x342E342E: idx=sc_5_1_4_34_4_4;break;//UnifiedProcedureStepEventTrial
            case 0x312E352E: idx=sc_5_1_4_34_5_1;break;//UPSFilteredGlobalSubscriptionInstance
            case 0x312E362E: idx=sc_5_1_4_34_6_1;break;//UnifiedProcedureStepPush
            case 0x322E362E: idx=sc_5_1_4_34_6_2;break;//UnifiedProcedureStepWatch
            case 0x332E362E: idx=sc_5_1_4_34_6_3;break;//UnifiedProcedureStepPull
            case 0x342E362E: idx=sc_5_1_4_34_6_4;break;//UnifiedProcedureStepEvent
            case 0x352E362E: idx=sc_5_1_4_34_6_5;break;//UnifiedProcedureStepQuery
         }};break;
         
      case 27:{
         switch ((valbytes[26] << 24) | (valbytes[25]<<16) | (valbytes[24]<<8) | valbytes[22]) {
            case 0x312E3131: idx=sc_5_1_4_1_1_1_1;break;//DigitalXRayImageStorageForPresentation
            case 0x322E3131: idx=sc_5_1_4_1_1_1_2;break;//DigitalMammographyXRayImageStorageForPresentation
            case 0x332E3131: idx=sc_5_1_4_1_1_1_3;break;//DigitalIntraOralXRayImageStorageForPresentation
            case 0x38323131: idx=sc_5_1_4_1_1_128;break;//PositronEmissionTomographyImageStorage
            case 0x39323131: idx=sc_5_1_4_1_1_129;break;//StandalonePETCurveStorage
            case 0x30333131: idx=sc_5_1_4_1_1_130;break;//EnhancedPETImageStorage
            case 0x31333131: idx=sc_5_1_4_1_1_131;break;//BasicStructuredDisplayStorage
            case 0x312E3231: idx=sc_5_1_4_1_1_2_1;break;//EnhancedCTImageStorage
            case 0x322E3231: idx=sc_5_1_4_1_1_2_2;break;//LegacyConvertedEnhancedCTImageStorage
            case 0x312E3331: idx=sc_5_1_4_1_1_3_1;break;//UltrasoundMultiFrameImageStorage
            case 0x312E3431: idx=sc_5_1_4_1_1_4_1;break;//EnhancedMRImageStorage
            case 0x322E3431: idx=sc_5_1_4_1_1_4_2;break;//MRSpectroscopyStorage
            case 0x332E3431: idx=sc_5_1_4_1_1_4_3;break;//EnhancedMRColorImageStorage
            case 0x342E3431: idx=sc_5_1_4_1_1_4_4;break;//LegacyConvertedEnhancedMRImageStorage
            case 0x312E3631: idx=sc_5_1_4_1_1_6_1;break;//UltrasoundImageStorage
            case 0x322E3631: idx=sc_5_1_4_1_1_6_2;break;//EnhancedUSVolumeStorage
            case 0x332E3631: idx=sc_5_1_4_1_1_6_3;break;//PhotoacousticImageStorage
            case 0x312E3731: idx=sc_5_1_4_1_1_7_1;break;//MultiFrameSingleBitSecondaryCaptureImageStorage
            case 0x322E3731: idx=sc_5_1_4_1_1_7_2;break;//MultiFrameGrayscaleByteSecondaryCaptureImageStorage
            case 0x332E3731: idx=sc_5_1_4_1_1_7_3;break;//MultiFrameGrayscaleWordSecondaryCaptureImageStorage
            case 0x342E3731: idx=sc_5_1_4_1_1_7_4;break;//MultiFrameTrueColorSecondaryCaptureImageStorage
            case 0x312E3931: idx=sc_5_1_4_1_1_9_1;break;//WaveformStorageTrial (Retired)
            case 0x312E3132: idx=sc_5_1_4_1_2_1_1;break;//PatientRootQueryRetrieveInformationModelFind
            case 0x322E3132: idx=sc_5_1_4_1_2_1_2;break;//PatientRootQueryRetrieveInformationModelMove
            case 0x332E3132: idx=sc_5_1_4_1_2_1_3;break;//PatientRootQueryRetrieveInformationModelGet
            case 0x312E3232: idx=sc_5_1_4_1_2_2_1;break;//StudyRootQueryRetrieveInformationModelFind
            case 0x322E3232: idx=sc_5_1_4_1_2_2_2;break;//StudyRootQueryRetrieveInformationModelMove
            case 0x332E3232: idx=sc_5_1_4_1_2_2_3;break;//StudyRootQueryRetrieveInformationModelGet
            case 0x312E3332: idx=sc_5_1_4_1_2_3_1;break;//PatientStudyOnlyQueryRetrieveInformationModelFind
            case 0x322E3332: idx=sc_5_1_4_1_2_3_2;break;//PatientStudyOnlyQueryRetrieveInformationModelMove
            case 0x332E3332: idx=sc_5_1_4_1_2_3_3;break;//PatientStudyOnlyQueryRetrieveInformationModelGet
            case 0x322E3432: idx=sc_5_1_4_1_2_4_2;break;//CompositeInstanceRootRetrieveMove
            case 0x332E3432: idx=sc_5_1_4_1_2_4_3;break;//CompositeInstanceRootRetrieveGet
            case 0x332E3532: idx=sc_5_1_4_1_2_5_3;break;//CompositeInstanceRetrieveWithoutBulkDataGet
         }};break;
         
      case 28:{
         switch ((valbytes[27] << 24) | (valbytes[26]<<16) | (valbytes[25]<<8) | valbytes[24]) {
            case 0x312E3131: idx=sc_5_1_4_1_1_11_1;break;//GrayscaleSoftcopyPresentationStateStorage
            case 0x322E3131: idx=sc_5_1_4_1_1_11_2;break;//ColorSoftcopyPresentationStateStorage
            case 0x332E3131: idx=sc_5_1_4_1_1_11_3;break;//PseudoColorSoftcopyPresentationStateStorage
            case 0x342E3131: idx=sc_5_1_4_1_1_11_4;break;//BlendingSoftcopyPresentationStateStorage
            case 0x352E3131: idx=sc_5_1_4_1_1_11_5;break;//XAXRFGrayscaleSoftcopyPresentationStateStorage
            case 0x362E3131: idx=sc_5_1_4_1_1_11_6;break;//GrayscalePlanarMPRVolumetricPresentationStateStorage
            case 0x372E3131: idx=sc_5_1_4_1_1_11_7;break;//CompositingPlanarMPRVolumetricPresentationStateStorage
            case 0x382E3131: idx=sc_5_1_4_1_1_11_8;break;//AdvancedBlendingPresentationStateStorage
            case 0x392E3131: idx=sc_5_1_4_1_1_11_9;break;//VolumeRenderingVolumetricPresentationStateStorage
            case 0x312E3231: idx=sc_5_1_4_1_1_12_1;break;//XRayAngiographicImageStorage
            case 0x322E3231: idx=sc_5_1_4_1_1_12_2;break;//XRayRadiofluoroscopicImageStorage
            case 0x332E3231: idx=sc_5_1_4_1_1_12_3;break;//XRayAngiographicBiPlaneImageStorage (Retired)
            case 0x312E3431: idx=sc_5_1_4_1_1_14_1;break;//IntravascularOpticalCoherenceTomographyImageStorageForPresentation
            case 0x322E3431: idx=sc_5_1_4_1_1_14_2;break;//IntravascularOpticalCoherenceTomographyImageStorageForProcessing
            case 0x312E3636: idx=sc_5_1_4_1_1_66_1;break;//SpatialRegistrationStorage
            case 0x322E3636: idx=sc_5_1_4_1_1_66_2;break;//SpatialFiducialsStorage
            case 0x332E3636: idx=sc_5_1_4_1_1_66_3;break;//DeformableSpatialRegistrationStorage
            case 0x342E3636: idx=sc_5_1_4_1_1_66_4;break;//SegmentationStorage
            case 0x352E3636: idx=sc_5_1_4_1_1_66_5;break;//SurfaceSegmentationStorage
            case 0x362E3636: idx=sc_5_1_4_1_1_66_6;break;//TractographyResultsStorage
            case 0x312E3836: idx=sc_5_1_4_1_1_68_1;break;//SurfaceScanMeshStorage
            case 0x322E3836: idx=sc_5_1_4_1_1_68_2;break;//SurfaceScanPointCloudStorage
            case 0x312E3737: idx=sc_5_1_4_1_1_77_1;break;//VLImageStorageTrial (Retired)
            case 0x322E3737: idx=sc_5_1_4_1_1_77_2;break;//VLMultiFrameImageStorageTrial (Retired)
            case 0x312E3837: idx=sc_5_1_4_1_1_78_1;break;//LensometryMeasurementsStorage
            case 0x322E3837: idx=sc_5_1_4_1_1_78_2;break;//AutorefractionMeasurementsStorage
            case 0x332E3837: idx=sc_5_1_4_1_1_78_3;break;//KeratometryMeasurementsStorage
            case 0x342E3837: idx=sc_5_1_4_1_1_78_4;break;//SubjectiveRefractionMeasurementsStorage
            case 0x352E3837: idx=sc_5_1_4_1_1_78_5;break;//VisualAcuityMeasurementsStorage
            case 0x362E3837: idx=sc_5_1_4_1_1_78_6;break;//SpectaclePrescriptionReportStorage
            case 0x372E3837: idx=sc_5_1_4_1_1_78_7;break;//OphthalmicAxialMeasurementsStorage
            case 0x382E3837: idx=sc_5_1_4_1_1_78_8;break;//IntraocularLensCalculationsStorage
            case 0x312E3937: idx=sc_5_1_4_1_1_79_1;break;//MacularGridThicknessAndVolumeReportStorage
            case 0x312E3038: idx=sc_5_1_4_1_1_80_1;break;//OphthalmicVisualFieldStaticPerimetryMeasurementsStorage
            case 0x312E3138: idx=sc_5_1_4_1_1_81_1;break;//OphthalmicThicknessMapStorage
            case 0x312E3238: idx=sc_5_1_4_1_1_82_1;break;//CornealTopographyMapStorage
            case 0x312E3838: idx=sc_5_1_4_1_1_88_1;break;//TextSRStorageTrial (Retired)
            case 0x322E3838: idx=sc_5_1_4_1_1_88_2;break;//AudioSRStorageTrial (Retired)
            case 0x332E3838: idx=sc_5_1_4_1_1_88_3;break;//DetailSRStorageTrial (Retired)
            case 0x342E3838: idx=sc_5_1_4_1_1_88_4;break;//ComprehensiveSRStorageTrial (Retired)
            case 0x312E3039: idx=sc_5_1_4_1_1_90_1;break;//ContentAssessmentResultsStorage
            case 0x312E3139: idx=sc_5_1_4_1_1_91_1;break;//MicroscopyBulkSimpleAnnotationsStorage
         }};break;

      case 29:{
         switch ((valbytes[28] << 24) | (valbytes[27]<<16) | (valbytes[26]<<8) | valbytes[24]) {
            case 0x312E3131: idx=sc_5_1_4_1_1_1_1_1;break;//DigitalXRayImageStorageForProcessing
            case 0x312E3231: idx=sc_5_1_4_1_1_1_2_1;break;//DigitalMammographyXRayImageStorageForProcessing
            case 0x312E3331: idx=sc_5_1_4_1_1_1_3_1;break;//DigitalIntraOralXRayImageStorageForProcessing
            case 0x312E3139: idx=sc_5_1_4_1_1_9_1_1;break;//TwelveLeadECGWaveformStorage
            case 0x322E3139: idx=sc_5_1_4_1_1_9_1_2;break;//GeneralECGWaveformStorage
            case 0x332E3139: idx=sc_5_1_4_1_1_9_1_3;break;//AmbulatoryECGWaveformStorage
            case 0x342E3139: idx=sc_5_1_4_1_1_9_1_4;break;//General32bitECGWaveformStorage
            case 0x312E3239: idx=sc_5_1_4_1_1_9_2_1;break;//HemodynamicWaveformStorage
            case 0x312E3339: idx=sc_5_1_4_1_1_9_3_1;break;//CardiacElectrophysiologyWaveformStorage
            case 0x312E3439: idx=sc_5_1_4_1_1_9_4_1;break;//BasicVoiceAudioWaveformStorage
            case 0x322E3439: idx=sc_5_1_4_1_1_9_4_2;break;//GeneralAudioWaveformStorage
            case 0x312E3539: idx=sc_5_1_4_1_1_9_5_1;break;//ArterialPulseWaveformStorage
            case 0x312E3639: idx=sc_5_1_4_1_1_9_6_1;break;//RespiratoryWaveformStorage
            case 0x322E3639: idx=sc_5_1_4_1_1_9_6_2;break;//MultichannelRespiratoryWaveformStorage
            case 0x312E3739: idx=sc_5_1_4_1_1_9_7_1;break;//RoutineScalpElectroencephalogramWaveformStorage
            case 0x322E3739: idx=sc_5_1_4_1_1_9_7_2;break;//ElectromyogramWaveformStorage
            case 0x332E3739: idx=sc_5_1_4_1_1_9_7_3;break;//ElectrooculogramWaveformStorage
            case 0x342E3739: idx=sc_5_1_4_1_1_9_7_4;break;//SleepElectroencephalogramWaveformStorage
            case 0x312E3839: idx=sc_5_1_4_1_1_9_8_1;break;//BodyPositionWaveformStorage
            case 0x30312E31: idx=sc_5_1_4_1_1_11_10;break;//SegmentedVolumeRenderingVolumetricPresentationStateStorage
            case 0x31312E31: idx=sc_5_1_4_1_1_11_11;break;//MultipleVolumeRenderingVolumetricPresentationStateStorage
            case 0x32312E31: idx=sc_5_1_4_1_1_11_12;break;//VariableModalityLUTSoftcopyPresentationStateStorage
            case 0x37372E31: idx=sc_5_1_4_1_1_12_77;break;//(Retired)
            case 0x31312E38: idx=sc_5_1_4_1_1_88_11;break;//BasicTextSRStorage
            case 0x32322E38: idx=sc_5_1_4_1_1_88_22;break;//EnhancedSRStorage
            case 0x33332E38: idx=sc_5_1_4_1_1_88_33;break;//ComprehensiveSRStorage
            case 0x34332E38: idx=sc_5_1_4_1_1_88_34;break;//Comprehensive3DSRStorage
            case 0x35332E38: idx=sc_5_1_4_1_1_88_35;break;//ExtensibleSRStorage
            case 0x30342E38: idx=sc_5_1_4_1_1_88_40;break;//ProcedureLogStorage
            case 0x30352E38: idx=sc_5_1_4_1_1_88_50;break;//MammographyCADSRStorage
            case 0x39352E38: idx=sc_5_1_4_1_1_88_59;break;//KeyObjectSelectionDocumentStorage
            case 0x35362E38: idx=sc_5_1_4_1_1_88_65;break;//ChestCADSRStorage
            case 0x37362E38: idx=sc_5_1_4_1_1_88_67;break;//XRayRadiationDoseSRStorage
            case 0x38362E38: idx=sc_5_1_4_1_1_88_68;break;//RadiopharmaceuticalRadiationDoseSRStorage
            case 0x39362E38: idx=sc_5_1_4_1_1_88_69;break;//ColonCADSRStorage
            case 0x30372E38: idx=sc_5_1_4_1_1_88_70;break;//ImplantationPlanSRStorage
            case 0x31372E38: idx=sc_5_1_4_1_1_88_71;break;//AcquisitionContextSRStorage
            case 0x32372E38: idx=sc_5_1_4_1_1_88_72;break;//SimplifiedAdultEchoSRStorage
            case 0x33372E38: idx=sc_5_1_4_1_1_88_73;break;//PatientRadiationDoseSRStorage
            case 0x34372E38: idx=sc_5_1_4_1_1_88_74;break;//PlannedImagingAgentAdministrationSRStorage
            case 0x35372E38: idx=sc_5_1_4_1_1_88_75;break;//PerformedImagingAgentAdministrationSRStorage
            case 0x36372E38: idx=sc_5_1_4_1_1_88_76;break;//EnhancedXRayRadiationDoseSRStorage
            case 0x312E3431: idx=sc_5_1_4_1_1_104_1;break;//EncapsulatedPDFStorage
            case 0x322E3431: idx=sc_5_1_4_1_1_104_2;break;//EncapsulatedCDAStorage
            case 0x332E3431: idx=sc_5_1_4_1_1_104_3;break;//EncapsulatedSTLStorage
            case 0x342E3431: idx=sc_5_1_4_1_1_104_4;break;//EncapsulatedOBJStorage
            case 0x352E3431: idx=sc_5_1_4_1_1_104_5;break;//EncapsulatedMTLStorage
            case 0x312E3831: idx=sc_5_1_4_1_1_128_1;break;//LegacyConvertedEnhancedPETImageStorage
            case 0x312E3032: idx=sc_5_1_4_1_1_200_1;break;//CTDefinedProcedureProtocolStorage
            case 0x322E3032: idx=sc_5_1_4_1_1_200_2;break;//CTPerformedProcedureProtocolStorage
            case 0x332E3032: idx=sc_5_1_4_1_1_200_3;break;//ProtocolApprovalStorage
            case 0x342E3032: idx=sc_5_1_4_1_1_200_4;break;//ProtocolApprovalInformationModelFind
            case 0x352E3032: idx=sc_5_1_4_1_1_200_5;break;//ProtocolApprovalInformationModelMove
            case 0x362E3032: idx=sc_5_1_4_1_1_200_6;break;//ProtocolApprovalInformationModelGet
            case 0x372E3032: idx=sc_5_1_4_1_1_200_7;break;//XADefinedProcedureProtocolStorage
            case 0x382E3032: idx=sc_5_1_4_1_1_200_8;break;//XAPerformedProcedureProtocolStorage
            case 0x312E3132: idx=sc_5_1_4_1_1_201_1;break;//InventoryStorage
            case 0x322E3132: idx=sc_5_1_4_1_1_201_2;break;//InventoryFind
            case 0x332E3132: idx=sc_5_1_4_1_1_201_3;break;//InventoryMove
            case 0x342E3132: idx=sc_5_1_4_1_1_201_4;break;//InventoryGet
            case 0x352E3132: idx=sc_5_1_4_1_1_201_5;break;//InventoryCreation
            case 0x362E3132: idx=sc_5_1_4_1_1_201_6;break;//RepositoryQuery
            case 0x312E3134: idx=sc_5_1_4_1_1_481_1;break;//RTImageStorage
            case 0x322E3134: idx=sc_5_1_4_1_1_481_2;break;//RTDoseStorage
            case 0x332E3134: idx=sc_5_1_4_1_1_481_3;break;//RTStructureSetStorage
            case 0x342E3134: idx=sc_5_1_4_1_1_481_4;break;//RTBeamsTreatmentRecordStorage
            case 0x352E3134: idx=sc_5_1_4_1_1_481_5;break;//RTPlanStorage
            case 0x362E3134: idx=sc_5_1_4_1_1_481_6;break;//RTBrachyTreatmentRecordStorage
            case 0x372E3134: idx=sc_5_1_4_1_1_481_7;break;//RTTreatmentSummaryRecordStorage
            case 0x382E3134: idx=sc_5_1_4_1_1_481_8;break;//RTIonPlanStorage
            case 0x392E3134: idx=sc_5_1_4_1_1_481_9;break;//RTIonBeamsTreatmentRecordStorage
            case 0x312E3135: idx=sc_5_1_4_1_1_501_1;break;//DICOSCTImageStorage
            case 0x332E3135: idx=sc_5_1_4_1_1_501_3;break;//DICOSThreatDetectionReportStorage
            case 0x342E3135: idx=sc_5_1_4_1_1_501_4;break;//DICOS2DAITStorage
            case 0x352E3135: idx=sc_5_1_4_1_1_501_5;break;//DICOS3DAITStorage
            case 0x362E3135: idx=sc_5_1_4_1_1_501_6;break;//DICOSQuadrupoleResonanceStorage
            case 0x312E3136: idx=sc_5_1_4_1_1_601_1;break;//EddyCurrentImageStorage
            case 0x322E3136: idx=sc_5_1_4_1_1_601_1;break;//EddyCurrentMultiFrameImageStorage
         }};break;
         
      case 30:{
         switch ((valbytes[29] << 24) | (valbytes[28]<<16) | (valbytes[27]<<8) | valbytes[25]) {
            case 0x312E3132: idx=sc_5_1_4_1_1_12_1_1;break;//EnhancedXAImageStorage
            case 0x312E3232: idx=sc_5_1_4_1_1_12_2_1;break;//EnhancedXRFImageStorage
            case 0x312E3133: idx=sc_5_1_4_1_1_13_1_1;break;//XRay3DAngiographicImageStorage
            case 0x322E3133: idx=sc_5_1_4_1_1_13_1_2;break;//XRay3DCraniofacialImageStorage
            case 0x332E3133: idx=sc_5_1_4_1_1_13_1_3;break;//BreastTomosynthesisImageStorage
            case 0x342E3133: idx=sc_5_1_4_1_1_13_1_4;break;//BreastProjectionXRayImageStorageForPresentation
            case 0x352E3133: idx=sc_5_1_4_1_1_13_1_5;break;//BreastProjectionXRayImageStorageForProcessing
            case 0x312E3137: idx=sc_5_1_4_1_1_77_1_1;break;//VLEndoscopicImageStorage
            case 0x322E3137: idx=sc_5_1_4_1_1_77_1_2;break;//VLMicroscopicImageStorage
            case 0x332E3137: idx=sc_5_1_4_1_1_77_1_3;break;//VLSlideCoordinatesMicroscopicImageStorage
            case 0x342E3137: idx=sc_5_1_4_1_1_77_1_4;break;//VLPhotographicImageStorage
            case 0x362E3137: idx=sc_5_1_4_1_1_77_1_6;break;//VLWholeSlideMicroscopyImageStorage
            case 0x372E3137: idx=sc_5_1_4_1_1_77_1_7;break;//DermoscopicPhotographyImageStorage
            case 0x382E3137: idx=sc_5_1_4_1_1_77_1_8;break;//ConfocalMicroscopyImageStorage
            case 0x392E3137: idx=sc_5_1_4_1_1_77_1_9;break;//ConfocalMicroscopyTiledPyramidalImageStorage
            case 0x30312E38: idx=sc_5_1_4_1_1_481_10;break;//RTPhysicianIntentStorage
            case 0x31312E38: idx=sc_5_1_4_1_1_481_11;break;//RTSegmentAnnotationStorage
            case 0x32312E38: idx=sc_5_1_4_1_1_481_12;break;//RTRadiationSetStorage
            case 0x33312E38: idx=sc_5_1_4_1_1_481_13;break;//CArmPhotonElectronRadiationStorage
            case 0x34312E38: idx=sc_5_1_4_1_1_481_14;break;//TomotherapeuticRadiationStorage
            case 0x35312E38: idx=sc_5_1_4_1_1_481_15;break;//RoboticArmRadiationStorage
            case 0x36312E38: idx=sc_5_1_4_1_1_481_16;break;//RTRadiationRecordSetStorage
            case 0x37312E38: idx=sc_5_1_4_1_1_481_17;break;//RTRadiationSalvageRecordStorage
            case 0x38312E38: idx=sc_5_1_4_1_1_481_18;break;//TomotherapeuticRadiationRecordStorage
            case 0x39312E38: idx=sc_5_1_4_1_1_481_19;break;//CArmPhotonElectronRadiationRecordStorage
            case 0x30322E38: idx=sc_5_1_4_1_1_481_20;break;//RoboticRadiationRecordStorage
            case 0x31322E38: idx=sc_5_1_4_1_1_481_21;break;//RTRadiationSetDeliveryInstructionStorage
            case 0x32322E38: idx=sc_5_1_4_1_1_481_22;break;//RTTreatmentPreparationStorage
            case 0x33322E38: idx=sc_5_1_4_1_1_481_23;break;//EnhancedRTImageStorage
            case 0x34322E38: idx=sc_5_1_4_1_1_481_24;break;//EnhancedContinuousRTImageStorage
            case 0x35322E38: idx=sc_5_1_4_1_1_481_25;break;//RTPatientPositionAcquisitionInstructionStorage
         }};break;

      case 31:{
         switch (valbytes[30]){
            case 0x31: idx=sc_5_1_4_1_1_501_2_1;break;//DICOSDigitalXRayImageStorageForPresentation
            case 0x32: idx=sc_5_1_4_1_1_501_2_2;break;//DICOSDigitalXRayImageStorageForProcessing
         }};break;
         
      case 32:{
         switch ((valbytes[31] << 24) | (valbytes[30]<<16) | (valbytes[29]<<8) | valbytes[28]) {
            case 0x312E312E: idx=sc_5_1_4_1_1_77_1_1_1;break;//VideoEndoscopicImageStorage
            case 0x312E322E: idx=sc_5_1_4_1_1_77_1_2_1;break;//VideoMicroscopicImageStorage
            case 0x312E342E: idx=sc_5_1_4_1_1_77_1_4_1;break;//VideoPhotographicImageStorage
            case 0x312E352E: idx=sc_5_1_4_1_1_77_1_5_1;break;//OphthalmicPhotography8BitImageStorage
            case 0x322E352E: idx=sc_5_1_4_1_1_77_1_5_2;break;//OphthalmicPhotography16BitImageStorage
            case 0x332E352E: idx=sc_5_1_4_1_1_77_1_5_3;break;//StereometricRelationshipStorage
            case 0x342E352E: idx=sc_5_1_4_1_1_77_1_5_4;break;//OphthalmicTomographyImageStorage
            case 0x352E352E: idx=sc_5_1_4_1_1_77_1_5_5;break;//WideFieldOphthalmicPhotographyStereographicProjectionImageStorage
            case 0x362E352E: idx=sc_5_1_4_1_1_77_1_5_6;break;//WideFieldOphthalmicPhotography3DCoordinatesImageStorage
            case 0x372E352E: idx=sc_5_1_4_1_1_77_1_5_7;break;//OphthalmicOpticalCoherenceTomographyEnFaceImageStorage
            case 0x382E352E: idx=sc_5_1_4_1_1_77_1_5_8;break;//OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage
         }};break;
      default:{
         
      };break;//error
   };
   
   if (strncmp(scstr[idx],(char*)valbytes,strlen(scstr[idx]))==0) return idx;
   else return 0;//error (verification)
}

NSString *scname( uint16 scidx)
{
   return [NSString stringWithUTF8String:scstr[scidx]];
}
