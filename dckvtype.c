//  dicm2dckv
//  log.c

#include "dckvtype.h"


#pragma mark - endianness


u32 u32swap(u32 x)
{
   return ((x>>24)&0xff)
         |((x<<8)&0xff0000)
         |((x>>8)&0xff00)
         |((x<<24)&0xff000000);
}

u16 u16swap(u16 x)
{
   return ((x<<8)&0xff00)
   |((x>>8)&0x00ff);
}

#pragma mark - main & log

enum DIWEFenum DIWEF;
bool loglevel(const char * logletter)
{
   switch ((int)*logletter) {
      case 'D':
         DIWEF=D;
         break;
      case 'I':
         DIWEF=I;
         break;
      case 'W':
         DIWEF=W;
         break;
      case 'E':
         DIWEF=E;
         break;
      case 'F':
         DIWEF=F;
         break;
      default:
         return false;
   }
   return true;
}


#pragma mark - repertoires

const char *repertoirestr[]={
   //one code
   "",                //empty
   "ISO_IR 100",      //latin1
   "ISO_IR 101",      //latin2
   "ISO_IR 109",      //latin3
   "ISO_IR 110",      //latin4
   "ISO_IR 148",      //latin5
   "ISO_IR 126",      //greek
   "ISO_IR 127",      //arabic

   "ISO_IR 192",      //utf-8 (multi byte)
   "RFC3986 ",         //for UR (not a DICOM defined code)
   "ISO_IR 13 ",      //japanese
   "ISO_IR 144",      //cyrilic
   "ISO_IR 138",      //hebrew
   "ISO_IR 166",      //thai
   "GB18030 ",        //chinese  (multi byte)
   "GBK ",            //chinese simplified  (multi byte)
   //code extension
   "ISO 2022 IR 6",  //default
   "ISO 2022 IR 100",//latin1
   "ISO 2022 IR 101",//latin2
   "ISO 2022 IR 109",//latin3
   "ISO 2022 IR 110",//latin4
   "ISO 2022 IR 148",//latin5
   "ISO 2022 IR 126",//greek
   "ISO 2022 IR 127",//arabic
   "ISO 2022 IR 87",  //japanese (multi byte)
   "ISO 2022 IR 159",//japanese (multi byte)
   "ISO 2022 IR 13",  //japanese
   "ISO 2022 IR 144",//cyrilic
   "ISO 2022 IR 138",//hebrew
   "ISO 2022 IR 166",//thai
   "ISO 2022 IR 149",//korean (multi byte)
   "ISO 2022 IR 58"   //chinese simplified (multi byte)
};

u32 repertoireidx( uint8_t *vbuf, u16 vallength )
{
   u32 idx=0x9;//error
   switch (vallength){
      case 0: idx=REPERTOIRE_GL;break;
      case 4: idx=GBK;break;
      case 8: idx=GB18030;break;
      case 10:{
         switch ((vbuf[9]<<8) | vbuf[8]) {
            case 0x3030: idx=ISO_IR100;break;//latin1
            case 0x3130: idx=ISO_IR101;break;//latin2
            case 0x3930: idx=ISO_IR109;break;//latin3
            case 0x3031: idx=ISO_IR110;break;//latin4
            case 0x3834: idx=ISO_IR148;break;//latin5
            case 0x3632: idx=ISO_IR126;break;//greek
            case 0x3732: idx=ISO_IR127;break;//arabic
            case 0x3239: idx=ISO_IR192;break;//utf-8
            case 0x2033: idx=ISO_IR13;break;//japones
               //return RFC3986
            case 0x3434: idx=ISO_IR144;break;//cyrilic
            case 0x3833: idx=ISO_IR138;break;//hebrew
            case 0x3636: idx=ISO_IR166;break;//thai
         }
      }
      case 14:{
#pragma mark TODO
      }
      case 16:{
#pragma mark TODO
      }
      default:{
#pragma mark TODO encoding extension
      }
   }
   if (strncmp(repertoirestr[idx],(char*)vbuf,vallength)==0) return idx;
   else return 0x9;//error
}



#pragma mark - SopClasses

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

u16 scidx( uint8_t *vbuf, u16 vallength )
{
   u16 idx=0x0;//verification / error
   switch (vallength-(vbuf[vallength-1]==0)){
         
      case 17: {
         switch (vbuf[16]){
            case 0x31: idx=sc_1_1;break;//Verification
            case 0x39: idx=sc_1_9;break;//BasicStudyContentNotification
         }};break;
         
      case 18: {
         switch ((vbuf[17]<<8) | vbuf[16]) {
            case 0x3034: idx=sc_1_40;break;//ProceduralEventLogging"
            case 0x3234: idx=sc_1_42;break;//SubstanceAdministrationLogging
            case 0x312E: idx=sc_10_1;break;//VideoEndoscopicImageRealTimeCommunication
            case 0x322E: idx=sc_10_2;break;//VideoPhotographicImageRealTimeCommunication
            case 0x332E: idx=sc_10_3;break;//AudioWaveformRealTimeCommunication
            case 0x342E: idx=sc_10_4;break;//RenditionSelectionDocumentRealTimeCommunication
         }};break;
         
      case 20:{
         switch (vbuf[19]){
            case 0x30: idx=sc_1_3_10;break;//MediaStorageDirectoryStorage
            case 0x31: idx=sc_1_20_1;break;//StorageCommitmentPushModel
            case 0x32: idx=sc_1_20_2;break;//StorageCommitmentPullModel (Retired)
         }};break;
         
      case 21:{
         switch (vbuf[20]){
            case 0x31: idx=sc_5_1_1_1;break;//BasicFilmSession
            case 0x32: idx=sc_5_1_1_2;break;//BasicFilmBox
            case 0x34: idx=sc_5_1_1_4;break;//BasicGrayscaleImageBox
            case 0x39: idx=sc_5_1_1_9;break;//BasicGrayscalePrintManagementMeta
        }};break;
         
      case 22:{
         switch ((vbuf[21] << 24) | (vbuf[20]<<16) | (vbuf[19]<<8) | vbuf[18]) {
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
         switch ((vbuf[22] << 24) | (vbuf[21]<<16) | (vbuf[20]<<8) | vbuf[19]) {
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
         switch ((vbuf[23] << 24) | (vbuf[22]<<16) | (vbuf[21]<<8) | vbuf[20]) {
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
         switch (vbuf[24]){
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
         switch ((vbuf[25] << 24) | (vbuf[24]<<16) | (vbuf[23]<<8) | vbuf[22]) {
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
         switch ((vbuf[26] << 24) | (vbuf[25]<<16) | (vbuf[24]<<8) | vbuf[22]) {
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
         switch ((vbuf[27] << 24) | (vbuf[26]<<16) | (vbuf[25]<<8) | vbuf[24]) {
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
         switch ((vbuf[28] << 24) | (vbuf[27]<<16) | (vbuf[26]<<8) | vbuf[24]) {
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
         switch ((vbuf[29] << 24) | (vbuf[28]<<16) | (vbuf[27]<<8) | vbuf[25]) {
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
         switch (vbuf[30]){
            case 0x31: idx=sc_5_1_4_1_1_501_2_1;break;//DICOSDigitalXRayImageStorageForPresentation
            case 0x32: idx=sc_5_1_4_1_1_501_2_2;break;//DICOSDigitalXRayImageStorageForProcessing
         }};break;
         
      case 32:{
         switch ((vbuf[31] << 24) | (vbuf[30]<<16) | (vbuf[29]<<8) | vbuf[28]) {
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
   
   if (strncmp(scstr[idx],(char*)vbuf,strlen(scstr[idx]))==0) return idx;
   else return 0;//error (verification)
}




#pragma mark - TransfertSyntaxes

const char *tsstr[]={
"1.2.840.10008.1.​20",        //Papyrus​3​Implicit​VR​Little​Endian      Retired
"1.2.840.10008.1.​2",         //Implicit​VR​Little​Endian
"1.2.840.10008.1.​2.​1",       //Explicit​VR​Little​Endian
"1.2.840.10008.1.​2.​1.98",    //Encapsulated​Uncompressed​Explicit​VR​Little​Endian
"1.2.840.10008.1.2.​1.​99",    //Deflated​Explicit​VR​Little​Endian
"1.2.840.10008.1.​2.​2",       //Explicit​VR​Big​Endian     Retired
"1.2.840.10008.1.2.​4.​50",    //JPEG​Baseline​8​Bit
"1.2.840.10008.1.2.​4.​51",    //JPEG​Extended​12​Bit
"1.2.840.10008.1.2.​4.​52",    //JPEG​Extended35      Retired
"1.2.840.10008.1.2.​4.​53",    //JPEG​Spectral​Selection​Non​Hierarchical68      Retired
"1.2.840.10008.1.2.​4.​54",    //JPEG​Spectral​Selection​Non​Hierarchical79      Retired
"1.2.840.10008.1.2.​4.​55",    //JPEG​Full​Progression​Non​Hierarchical1012      Retired
"1.2.840.10008.1.2.​4.​56",    //JPEG​Full​Progression​Non​Hierarchical1113      Retired
"1.2.840.10008.1.2.​4.​57",    //JPEG​Lossless
"1.2.840.10008.1.2.​4.​58",    //JPEG​Lossless​Non​Hierarchical​15      Retired
"1.2.840.10008.1.2.​4.​59",    //JPEG​Extended​Hierarchical​1618     Retired
"1.2.840.10008.1.2.​4.​60",    //JPEG​Extended​Hierarchical​1719     Retired
"1.2.840.10008.1.2.​4.​61",    //JPEG​Spectral​Selection​Hierarchical2022     Retired
"1.2.840.10008.1.2.​4.​62",    //JPEG​Spectral​Selection​Hierarchical2123     Retired
"1.2.840.10008.1.2.​4.​63",    //JPEG​Full​Progression​Hierarchical2426    Retired
"1.2.840.10008.1.2.​4.​64",    //JPEG​Full​Progression​Hierarchical2527    Retired
"1.2.840.10008.1.2.​4.​65",    //JPEG​Lossless​Hierarchical​28    Retired
"1.2.840.10008.1.2.​4.​66",    //JPEG​Lossless​Hierarchical​29    Retired
"1.2.840.10008.1.2.​4.​70",    //JPEG​Lossless​SV1
"1.2.840.10008.1.2.​4.​80",    //JPEG​LS​Lossless
"1.2.840.10008.1.2.​4.​81",    //JPEG​LS​Near​Lossless
"1.2.840.10008.1.2.​4.​90",    //JPEG​2000Lossless
"1.2.840.10008.1.2.​4.​91",    //JPEG​2000
"1.2.840.10008.1.2.​4.​92",    //JPEG​2000MCLossless
"1.2.840.10008.1.2.​4.​93",    //JPEG​2000MC
"1.2.840.10008.1.2.​4.​94",    //JPIP​Referenced
"1.2.840.10008.1.2.​4.​95",    //JPIP​Referenced​Deflate
"1.2.840.10008.1.2.​4.​100",  //MPEG2​MPML
"1.2.840.10008.1.2.​4.​100.1",//MPEG2​MPMLF
"1.2.840.10008.1.2.​4.​101",  //MPEG2​MPHL
"1.2.840.10008.1.2.​4.​101.1",//MPEG2​MPHLF
"1.2.840.10008.1.2.​4.​102",  //MPEG4​HP41
"1.2.840.10008.1.2.​4.​102.1",//MPEG4​HP41F
"1.2.840.10008.1.2.​4.​103",  //MPEG4​HP41BD
"1.2.840.10008.1.2.​4.​103.1",//MPEG4​HP41BDF
"1.2.840.10008.1.2.​4.​104",  //MPEG4​HP422D
"1.2.840.10008.1.2.​4.​104.1",//MPEG4​HP422DF
"1.2.840.10008.1.2.​4.​105",  //MPEG4​HP423D
"1.2.840.10008.1.2.​4.​105.1",//MPEG4​HP423DF
"1.2.840.10008.1.2.​4.​106",  //MPEG4​HP42STEREO
"1.2.840.10008.1.2.​4.​106.1",//MPEG4​HP42STEREOF
"1.2.840.10008.1.2.​4.107",  //HEVC​MP51
"1.2.840.10008.1.2.​4.108",  //HEVC​M10P51
"1.2.840.10008.1.2.​4.​201",  //HTJ2K​Lossless
"1.2.840.10008.1.2.​4.202",  //HTJ2K​Lossless​RPCL
"1.2.840.10008.1.2.​4.​203",  //HTJ2K
"1.2.840.10008.1.2.​4.​204",  //JPIP​HTJ2K​Referenced
"1.2.840.10008.1.2.​4.​205",  //JPIP​HTJ2K​Referenced​Deflate
"1.2.840.10008.1.​2.​5",      //RLE​Lossless
"1.2.840.10008.1.2.​6.​1",    //RFC​2557​MIME​Encapsulation
"1.2.840.10008.1.2.​6.​2",    //XML​Encoding
"1.2.840.10008.1.2.7.1",    //SMPTE​ST​211020​Uncompressed​Progressive​Active​Video
"1.2.840.10008.1.2.7.2",    //SMPTE​ST​211020​Uncompressed​Interlaced​Active​Video
"1.2.840.10008.1.2.7.3"     //SMPTE​ST​211030​PCMDigital​Audio
};

//ts_papyrus3ile = 0 = código de error
u8 tsidx( uint8_t *vbuf, u16 vallength )
{
   u8 idx=0x0;//verification / error
   
   switch (vallength){
      case 17: idx=ts_;break;//Implicit​VR​Little​Endian
      case 19:{
         switch (vbuf[18]) {
            case 0x31: idx=ts_1;break;//Explicit​VR​Little​Endian
            case 0x32: idx=ts_2;break;//Explicit​VR​Big​Endian     Retired
            case 0x35: idx=ts_5;break;//RLE​Lossless
            default : idx=ts_papyrus3ile;break;//error
         }};break;
      case 22:{
         switch ((vbuf[20]<<8)+vbuf[21]) {
            case 0x3938: idx=ts_1_98;break;//Encapsulated​Uncompressed​Explicit​VR​Little​Endian
            case 0x3939: idx=ts_1_99;break;//Deflated​Explicit​VR​Little​Endian
            case 0x3035: idx=ts_4_50;break;//JPEG​Baseline​8​Bit
            case 0x3135: idx=ts_4_51;break;//JPEG​Extended​12​Bit
            case 0x3235: idx=ts_4_52;break;//JPEG​Extended35      Retired
            case 0x3335: idx=ts_4_53;break;//JPEG​Spectral​Selection​Non​Hierarchical68      Retired
            case 0x3435: idx=ts_4_54;break;//JPEG​Spectral​Selection​Non​Hierarchical79      Retired
            case 0x3535: idx=ts_4_55;break;//JPEG​Full​Progression​Non​Hierarchical1012      Retired
            case 0x3635: idx=ts_4_56;break;//JPEG​Full​Progression​Non​Hierarchical1113      Retired
            case 0x3735: idx=ts_4_57;break;//JPEG​Lossless
            case 0x3835: idx=ts_4_58;break;//JPEG​Lossless​Non​Hierarchical​15      Retired
            case 0x3935: idx=ts_4_59;break;//JPEG​Extended​Hierarchical​1618     Retired
            case 0x3036: idx=ts_4_60;break;//JPEG​Extended​Hierarchical​1719     Retired
            case 0x3136: idx=ts_4_61;break;//JPEG​Spectral​Selection​Hierarchical2022     Retired
            case 0x3236: idx=ts_4_62;break;//JPEG​Spectral​Selection​Hierarchical2123     Retired
            case 0x3336: idx=ts_4_63;break;//JPEG​Full​Progression​Hierarchical2426    Retired
            case 0x3436: idx=ts_4_64;break;//JPEG​Full​Progression​Hierarchical2527    Retired
            case 0x3536: idx=ts_4_65;break;//JPEG​Lossless​Hierarchical​28    Retired
            case 0x3636: idx=ts_4_66;break;//JPEG​Lossless​Hierarchical​29    Retired
            case 0x3037: idx=ts_4_70;break;//JPEG​Lossless​SV1
            case 0x3038: idx=ts_4_80;break;//JPEG​LS​Lossless
            case 0x3138: idx=ts_4_81;break;//JPEG​LS​Near​Lossless
            case 0x3039: idx=ts_4_90;break;//JPEG​2000Lossless
            case 0x3139: idx=ts_4_91;break;//JPEG​2000
            case 0x3239: idx=ts_4_92;break;//JPEG​2000MCLossless
            case 0x3339: idx=ts_4_93;break;//JPEG​2000MC
            case 0x3439: idx=ts_4_94;break;//JPIP​Referenced
            case 0x3539: idx=ts_4_95;break;//JPIP​Referenced​Deflate
            case 0x2031:{
               switch (vbuf[18]){
                  case 0x36: idx=ts_6_1;break;//RFC​2557​MIME​Encapsulation
                  case 0x37: idx=ts_7_1;break;//SMPTE​ST​211020​Uncompressed​Progressive​Active​Video
                  default: idx=ts_papyrus3ile;break;//error
               }};break;
            case 0x2032:{
               switch (vbuf[18]){
                  case 0x36: idx=ts_6_2;break;//XML​Encoding
                  case 0x37: idx=ts_7_2;break;//SMPTE​ST​211020​Uncompressed​Interlaced​Active​Video
                  default: idx=ts_papyrus3ile;break;//error
               }};break;
            case 0x2033: idx=ts_7_3;break;//SMPTE​ST​211030​PCMDigital​Audio
            default    : idx=ts_papyrus3ile;break;//error
         }};break;
      case 23:{
         switch (vbuf[20]){
            case 31:{
               switch (vbuf[22]){
                  case 0x30:idx=ts_4_100 ;break;//MPEG2​MPML
                  case 0x31:idx=ts_4_101 ;break;//MPEG2​MPHL
                  case 0x32:idx=ts_4_102 ;break;//MPEG4​HP41
                  case 0x33:idx=ts_4_103 ;break;//MPEG4​HP41BD
                  case 0x34:idx=ts_4_104 ;break;//MPEG4​HP422D
                  case 0x35:idx=ts_4_105 ;break;//MPEG4​HP423D
                  case 0x36:idx=ts_4_106 ;break;//MPEG4​HP42STEREO
                  case 0x37:idx=ts_4_107 ;break;//HEVC​MP51
                  case 0x38:idx=ts_4_108 ;break;//HEVC​M10P51
                  default: idx=ts_papyrus3ile;break;//error
               }};break;
            case 32:{
               switch (vbuf[22]){
                  case 0x31:idx=ts_4_201 ;break;//HTJ2K​Lossless
                  case 0x32:idx=ts_4_202 ;break;//HTJ2K​Lossless​RPCL
                  case 0x33:idx=ts_4_203 ;break;//HTJ2K
                  case 0x34:idx=ts_4_204 ;break;//JPIP​HTJ2K​Referenced
                  case 0x35:idx=ts_4_205 ;break;//JPIP​HTJ2K​Referenced​Deflate
                  default: idx=ts_papyrus3ile;break;//error
               }};break;
            default: idx=ts_papyrus3ile;break;//error
         }};break;
      case 25:{
         switch (vbuf[22]){
            case 0x30:idx=ts_4_100_1 ;break;//MPEG2​MPMLF
            case 0x31:idx=ts_4_101_1 ;break;//MPEG2​MPHLF
            case 0x32:idx=ts_4_102_1 ;break;//MPEG4​HP41F
            case 0x33:idx=ts_4_103_1 ;break;//MPEG4​HP41BDF
            case 0x34:idx=ts_4_104_1 ;break;//MPEG4​HP422DF
            case 0x35:idx=ts_4_105_1 ;break;//MPEG4​HP423DF
            case 0x36:idx=ts_4_106_1 ;break;//MPEG4​HP42STEREOF
            default: idx=ts_papyrus3ile;break;//error
         }
      };break;
      default: idx=ts_papyrus3ile;break;//error
   };
   
   if (strncmp(tsstr[idx],(char*)vbuf,strlen(tsstr[idx]))) return idx;
   else return 0;//error (verification)
}




#pragma mark - eDCKVprefixtag


#pragma mark base dataset level patient
/*
const u32 B00081120P=0x20110800;//SQ Referenced Patient Sequence
const u32 B00100010P=0x10001000;//PN Patient​Name
const u32 B00100020P=0x20001000;//LO Patient​ID
const u32 B00100021P=0x21001000;//LO Issuer​Of​Patient​ID
const u32 B00100022P=0x22001000;//CS Type​Of​Patient​ID
const u32 B00100024P=0x24001000;//SQ Issuer​Of​Patient​ID​Qualifiers​Sequence
const u32 B00100026P=0x26001000;//SQ Source​Patient​Group​Identification​Sequence
const u32 B00100027P=0x27001000;//SQ Group​Of​Patients​Identification​Sequence
const u32 B00100030P=0x30001000;//DA Patient​Birth​Date
const u32 B00100032P=0x32001000;//TM Patient​Birth​Time
const u32 B00100033P=0x33001000;//LO Patient​Birth​Date​In​Alternative​Calendar
const u32 B00100034P=0x34001000;//LO Patient​Death​Date​In​Alternative​Calendar
const u32 B00100035P=0x35001000;//CS Patient​Alternative​Calendar
const u32 B00100040P=0x40001000;//CS Patient​Sex
const u32 B00100200P=0x00021000;//CS Quality​Control​Subject
const u32 B00100212P=0x12021000;//UC Strain​Description
const u32 B00100213P=0x13021000;//LO Strain​Nomenclature
const u32 B00100216P=0x16021000;//SQ Strain​Stock​Sequence
const u32 B00100218P=0x18021000;//UT Strain​Additional​Information
const u32 B00100219P=0x19021000;//SQ Strain​Code​Sequence
const u32 B00100221P=0x21021000;//SQ Genetic​Modifications​​Sequence
const u32 B00101000P=0x00101000;//LO Other​Patient​IDs
const u32 B00101001P=0x01101000;//PN Other​Patient​Names
const u32 B00101002P=0x02101000;//SQ Other​Patient​IDs​Sequence
const u32 B00101100P=0x00111000;//SQ Referenced​Patient​Photo​Sequence
const u32 B00102160P=0x60211000;//SH Ethnic​Group
const u32 B00102201P=0x01221000;//LO Patient​Species​Description
const u32 B00102202P=0x02221000;//SQ Patient​Species​Code​Sequence
const u32 B00102292P=0x92221000;//LO Patient​Breed​Description
const u32 B00102293P=0x93221000;//SQ Patient​Breed​Code​Sequence
const u32 B00102294P=0x94221000;//SQ Breed​Registration​Sequence
const u32 B00102297P=0x97221000;//PN Responsible​Person
const u32 B00102298P=0x98221000;//CS Responsible​Person​Role
const u32 B00102299P=0x99221000;//LO Responsible​Organization
const u32 B00104000P=0x00401000;//LT Patient​Comments
const u32 B00120010P=0x10001200;//LO Clinical​Trial​Sponsor​Name
const u32 B00120020P=0x20001200;//LO Clinical​Trial​Protocol​ID
const u32 B00120021P=0x21001200;//LO Clinical​Trial​Protocol​Name
const u32 B00120030P=0x30001200;//LO Clinical​Trial​Site​ID
const u32 B00120031P=0x31001200;//LO Clinical​Trial​Site​Name
const u32 B00120040P=0x40001200;//LO Clinical​Trial​Subject​ID
const u32 B00120042P=0x42001200;//LO Clinical​Trial​Subject​Reading​ID
const u32 B00120062P=0x62001200;//CS Patient​Identity​Removed
const u32 B00120063P=0x63001200;//LO Deidentification​Method
const u32 B00120064P=0x64001200;//SQ Deidentification​Method​Code​Sequence
const u32 B00120081P=0x81001200;//LO Clinical​Trial​Protocol​Ethics​Committee​Name
const u32 B00120082P=0x82001200;//LO Clinical​Trial​Protocol​Ethics​Committee​Approval​Number
*/
const u32 L00081120P=0x00081120;//SQ Referenced Patient Sequence
const u32 L00100010P=0x00100010;//PN Patient​Name
const u32 L00100020P=0x00100020;//LO Patient​ID
const u32 L00100021P=0x00100021;//LO Issuer​Of​Patient​ID
const u32 L00100022P=0x00100022;//CS Type​Of​Patient​ID
const u32 L00100024P=0x00100024;//SQ Issuer​Of​Patient​ID​Qualifiers​Sequence
const u32 L00100026P=0x00100026;//SQ Source​Patient​Group​Identification​Sequence
const u32 L00100027P=0x00100027;//SQ Group​Of​Patients​Identification​Sequence
const u32 L00100030P=0x00100030;//DA Patient​Birth​Date
const u32 L00100032P=0x00100032;//TM Patient​Birth​Time
const u32 L00100033P=0x00100033;//LO Patient​Birth​Date​In​Alternative​Calendar
const u32 L00100034P=0x00100034;//LO Patient​Death​Date​In​Alternative​Calendar
const u32 L00100035P=0x00100035;//CS Patient​Alternative​Calendar
const u32 L00100040P=0x00100040;//CS Patient​Sex
const u32 L00100200P=0x00100200;//CS Quality​Control​Subject
const u32 L00100212P=0x00100212;//UC Strain​Description
const u32 L00100213P=0x00100213;//LO Strain​Nomenclature
const u32 L00100216P=0x00100216;//SQ Strain​Stock​Sequence
const u32 L00100218P=0x00100218;//UT Strain​Additional​Information
const u32 L00100219P=0x00100219;//SQ Strain​Code​Sequence
const u32 L00100221P=0x00100221;//SQ Genetic​Modifications​​Sequence
const u32 L00101000P=0x00101000;//LO Other​Patient​IDs
const u32 L00101001P=0x00101001;//PN Other​Patient​Names
const u32 L00101002P=0x00101002;//SQ Other​Patient​IDs​Sequence
const u32 L00101100P=0x00101100;//SQ Referenced​Patient​Photo​Sequence
const u32 L00102160P=0x00102160;//SH Ethnic​Group
const u32 L00102201P=0x00102201;//LO Patient​Species​Description
const u32 L00102202P=0x00102202;//SQ Patient​Species​Code​Sequence
const u32 L00102292P=0x00102292;//LO Patient​Breed​Description
const u32 L00102293P=0x00102293;//SQ Patient​Breed​Code​Sequence
const u32 L00102294P=0x00102294;//SQ Breed​Registration​Sequence
const u32 L00102297P=0x00102297;//PN Responsible​Person
const u32 L00102298P=0x00102298;//CS Responsible​Person​Role
const u32 L00102299P=0x00102299;//LO Responsible​Organization
const u32 L00104000P=0x00104000;//LT Patient​Comments
const u32 L00120010P=0x00120010;//LO Clinical​Trial​Sponsor​Name
const u32 L00120020P=0x00120020;//LO Clinical​Trial​Protocol​ID
const u32 L00120021P=0x00120021;//LO Clinical​Trial​Protocol​Name
const u32 L00120030P=0x00120030;//LO Clinical​Trial​Site​ID
const u32 L00120031P=0x00120031;//LO Clinical​Trial​Site​Name
const u32 L00120040P=0x00120040;//LO Clinical​Trial​Subject​ID
const u32 L00120042P=0x00120042;//LO Clinical​Trial​Subject​Reading​ID
const u32 L00120062P=0x00120062;//CS Patient​Identity​Removed
const u32 L00120063P=0x00120063;//LO Deidentification​Method
const u32 L00120064P=0x00120064;//SQ Deidentification​Method​Code​Sequence
const u32 L00120081P=0x00120081;//LO Clinical​Trial​Protocol​Ethics​Committee​Name
const u32 L00120082P=0x00120082;//LO Clinical​Trial​Protocol​Ethics​Committee​Approval​Number


#pragma mark base dataset level study
/*
const u32 B00080020E=0x20000800;//DA Study​Date
const u32 B00080030E=0x30000800;//TM Study​Time
const u32 B00080050E=0x50000800;//SH Accession​Number
const u32 B00080051E=0x51000800;//SQ Issuer​Of​Accession​Number​Sequence
const u32 B00080090E=0x90000800;//PN Referring​Physician​Name
const u32 B00080096E=0x96000800;//SQ Referring​Physician​Identification​Sequence
const u32 B0008009CE=0x9C000800;//PN Consulting​Physician​Name
const u32 B0008009DE=0x9D000800;//SQ Consulting​Physician​Identification​Sequence
const u32 B00081030E=0x30100800;//LO Study​Description
const u32 B00081032E=0x32100800;//SQ Procedure​Code​Sequence
const u32 B00081048E=0x48100800;//PN Physicians​Of​Record
const u32 B00081049E=0x49100800;//SQ Physicians​Of​Record​Identification​Sequence
const u32 B00081060E=0x60100800;//PN Name​Of​Physicians​Reading​Study
const u32 B00081062E=0x62100800;//SQ Physicians​Reading​Study​Identification​Sequence
const u32 B00081080E=0x80100800;//LO Admitting​Diagnoses​Description
const u32 B00081084E=0x84100800;//SQ Admitting​Diagnoses​Code​Sequence
const u32 B00081110E=0x10110800;//SQ Referenced​Study​Sequence
const u32 B00101010E=0x10101000;//AS Patient​Age
const u32 B00101020E=0x20101000;//DS Patient​Size
const u32 B00101021E=0x21101000;//SQ Patient​Size​Code​Sequence
const u32 B00101022E=0x22101000;//DS Patient​Body​Mass​Index
const u32 B00101023E=0x23101000;//DS Measured​APDimension
const u32 B00101024E=0x24101000;//DS Measured​Lateral​Dimension
const u32 B00101030E=0x30101000;//DS Patient​Weight
const u32 B00102000E=0x00201000;//LO Medical​Alerts
const u32 B00102110E=0x10211000;//LO Allergies
const u32 B00102180E=0x80211000;//SH Occupation
const u32 B001021A0E=0xA0211000;//CS Smoking​Status
const u32 B001021B0E=0xB0211000;//LT Additional​Patient​History
const u32 B001021C0E=0xC0211000;//US Pregnancy​Status
const u32 B001021D0E=0xD0211000;//DA Last​Menstrual​Date
const u32 B00102203E=0x03221000;//CS Patient​Sex​Neutered
const u32 B00120050E=0x50001200;//LO Clinical​Trial​Time​Point​ID
const u32 B00120051E=0x51001200;//ST Clinical​Trial​Time​Point​Description
const u32 B00120052E=0x52001200;//FD Longitudinal​Temporal​Offset​From​Event
const u32 B00120053E=0x53001200;//CS Longitudinal​Temporal​Event​Type
const u32 B00120054E=0x54001200;//SQ Clinical​Trial​Time​Point​Type​Code​Sequence
const u32 B00120083E=0x83001200;//SQ Consent​For​Clinical​Trial​Use​Sequence
const u32 B0020000DE=0x0D002000;//UI Study​Instance​UID
const u32 B00200010E=0x10002000;//SH Study​ID
const u32 B00321033E=0x33103200;//LO Requesting​Service
const u32 B00321034E=0x34103200;//SQ Requesting​Service​Code​Sequence
const u32 B00321066E=0x66103200;//UT Reason​For​Visit
const u32 B00321067E=0x67103200;//SQ Reason​For​Visit​Code​Sequence
const u32 B00380010E=0x10003800;//LO Admission​ID
const u32 B00380014E=0x14003800;//SQ Issuer​Of​Admission​ID​Sequence
const u32 B00380060E=0x60003800;//LO Service​Episode​ID
const u32 B00380062E=0x62003800;//LO Service​Episode​Description
const u32 B00380064E=0x64003800;//SQ Issuer​Of​Service​Episode​ID​Sequence
const u32 B00385000E=0x00053800;//LO Patient​State
const u32 B00401012E=0x12104000;//SQ Reason​For​Performed​Procedure​Code​Sequence
*/
const u32 L00080020E=0x00080020;//DA Study​Date
const u32 L00080030E=0x00080030;//TM Study​Time
const u32 L00080050E=0x00080050;//SH Accession​Number
const u32 L00080051E=0x00080051;//SQ Issuer​Of​Accession​Number​Sequence
const u32 L00080090E=0x00080090;//PN Referring​Physician​Name
const u32 L00080096E=0x00080096;//SQ Referring​Physician​Identification​Sequence
const u32 L0008009CE=0x0008009C;//PN Consulting​Physician​Name
const u32 L0008009DE=0x0008009D;//SQ Consulting​Physician​Identification​Sequence
const u32 L00081030E=0x00081030;//LO Study​Description
const u32 L00081032E=0x00081032;//SQ Procedure​Code​Sequence
const u32 L00081048E=0x00081048;//PN Physicians​Of​Record
const u32 L00081049E=0x00081049;//SQ Physicians​Of​Record​Identification​Sequence
const u32 L00081060E=0x00081060;//PN Name​Of​Physicians​Reading​Study
const u32 L00081062E=0x00081062;//SQ Physicians​Reading​Study​Identification​Sequence
const u32 L00081080E=0x00081080;//LO Admitting​Diagnoses​Description
const u32 L00081084E=0x00081084;//SQ Admitting​Diagnoses​Code​Sequence
const u32 L00081110E=0x00081110;//SQ Referenced​Study​Sequence
const u32 L00101010E=0x00101010;//AS Patient​Age
const u32 L00101020E=0x00101020;//DS Patient​Size
const u32 L00101021E=0x00101021;//SQ Patient​Size​Code​Sequence
const u32 L00101022E=0x00101022;//DS Patient​Body​Mass​Index
const u32 L00101023E=0x00101023;//DS Measured​APDimension
const u32 L00101024E=0x00101024;//DS Measured​Lateral​Dimension
const u32 L00101030E=0x00101030;//DS Patient​Weight
const u32 L00102000E=0x00102000;//LO Medical​Alerts
const u32 L00102110E=0x00102110;//LO Allergies
const u32 L00102180E=0x00102180;//SH Occupation
const u32 L001021A0E=0x001021A0;//CS Smoking​Status
const u32 L001021B0E=0x001021B0;//LT Additional​Patient​History
const u32 L001021C0E=0x001021C0;//US Pregnancy​Status
const u32 L001021D0E=0x001021D0;//DA Last​Menstrual​Date
const u32 L00102203E=0x00102203;//CS Patient​Sex​Neutered
const u32 L00120050E=0x00120050;//LO Clinical​Trial​Time​Point​ID
const u32 L00120051E=0x00120051;//ST Clinical​Trial​Time​Point​Description
const u32 L00120052E=0x00120052;//FD Longitudinal​Temporal​Offset​From​Event
const u32 L00120053E=0x00120053;//CS Longitudinal​Temporal​Event​Type
const u32 L00120054E=0x00120054;//SQ Clinical​Trial​Time​Point​Type​Code​Sequence
const u32 L00120083E=0x00120083;//SQ Consent​For​Clinical​Trial​Use​Sequence
const u32 L0020000DE=0x0020000D;//UI Study​Instance​UID
const u32 L00200010E=0x00200010;//SH Study​ID
const u32 L00321033E=0x00321033;//LO Requesting​Service
const u32 L00321034E=0x00321034;//SQ Requesting​Service​Code​Sequence
const u32 L00321066E=0x00321066;//UT Reason​For​Visit
const u32 L00321067E=0x00321067;//SQ Reason​For​Visit​Code​Sequence
const u32 L00380010E=0x00380010;//LO Admission​ID
const u32 L00380014E=0x00380014;//SQ Issuer​Of​Admission​ID​Sequence
const u32 L00380060E=0x00380060;//LO Service​Episode​ID
const u32 L00380062E=0x00380062;//LO Service​Episode​Description
const u32 L00380064E=0x00380064;//SQ Issuer​Of​Service​Episode​ID​Sequence
const u32 L00385000E=0x00385000;//LO Patient​State
const u32 L00401012E=0x00401012;//SQ Reason​For​Performed​Procedure​Code​Sequence


#pragma mark base dataset level series root
/*
const u32 B00080021S=0x21000800;//DA Series​Date
const u32 B00080031S=0x31000800;//TM Series​Time
const u32 B00080060S=0x60000800;//CS Modality
const u32 B00080064S=0x64000800;//CS Conversion​Type
const u32 B00080068S=0x68000800;//CS Presentation​Intent​Type
const u32 B00080070S=0x70000800;//LO Manufacturer
const u32 B00080080S=0x80000800;//LO Institution​Name
const u32 B00080081S=0x81000800;//ST Institution​Address
const u32 B00081010S=0x10100800;//SH Station​Name
const u32 B0008103ES=0x3E100800;//LO Series​Description
const u32 B0008103FS=0x3F100800;//SQ Series​Description​Code​Sequence
const u32 B00081040S=0x40100800;//LO Institutional​Department​Name
const u32 B00081041S=0x41100800;//SQ Institutional​Department​Type​Code​Sequence
const u32 B00081050S=0x50100800;//PN Performing​Physician​Name
const u32 B00081052S=0x52100800;//SQ Performing​Physician​Identification​Sequence
const u32 B00081070S=0x70100800;//PN Operators​Name
const u32 B00081072S=0x72100800;//SQ Operator​Identification​Sequence
const u32 B00081090S=0x90100800;//LO Manufacturer​Model​Name
const u32 B00081111S=0x11110800;//SQ Referenced​Performed​Procedure​Step​Sequence
const u32 B00081250S=0x50120800;//SQ Related​Series​Sequence
const u32 B00120060S=0x60001200;//LO Clinical​Trial​Coordinating​Center​Name
const u32 B00120071S=0x71001200;//LO Clinical​Trial​Series​ID
const u32 B00120072S=0x72001200;//LO Clinical​Trial​Series​Description
const u32 B0016004DS=0x4D001600;//UT Camera​Owner​Name
const u32 B0016004ES=0x4E001600;//DS Lens​Specification
const u32 B0016004FS=0x4F001600;//UT Lens​Make
const u32 B00160050S=0x50001600;//UT Lens​Model
const u32 B00160051S=0x51001600;//UT Lens​Serial​Number
const u32 B00180015S=0x15001800;//CS Body​Part​Examined
const u32 B00180026S=0x26001800;//SQ Intervention​Drug​Information​Sequence
const u32 B00180071S=0x71001800;//CS Acquisition​Termination​Condition
const u32 B00180073S=0x73001800;//CS Acquisition​Start​Condition
const u32 B00180074S=0x74001800;//IS Acquisition​Start​Condition​Data
const u32 B00180075S=0x75001800;//IS Acquisition​Termination​Condition​Data
const u32 B00181000S=0x00101800;//LO Device​Serial​Number
const u32 B00181002S=0x02101800;//UI Device​UID
const u32 B00181008S=0x08101800;//LO Gantry​ID
const u32 B0018100AS=0x0A101800;//SQ UDI​Sequence
const u32 B0018100BS=0x0B101800;//UI Manufacturer​Device​Class​UID
const u32 B00181010S=0x10101800;//LO Secondary​Capture​Device​ID
const u32 B00181016S=0x16101800;//LO Secondary​Capture​Device​Manufacturer
const u32 B00181018S=0x18101800;//LO Secondary​Capture​Device​Manufacturer​Model​Name
const u32 B00181019S=0x19101800;//LO Secondary​Capture​Device​Software​Versions
const u32 B00181020S=0x20101800;//LO Software​Versions
const u32 B00181022S=0x22101800;//SH Video​Image​Format​Acquired
const u32 B00181023S=0x23101800;//LO Digital​Image​Format​Acquired
const u32 B00181030S=0x30101800;//LO Protocol​Name
const u32 B00181050S=0x50101800;//DS Spatial​Resolution
const u32 B00181061S=0x61101800;//LO Trigger​Source​Or​Type
const u32 B00181064S=0x64101800;//LO Cardiac​Framing​Type
const u32 B0018106AS=0x6A101800;//CS Synchronization​Trigger
const u32 B0018106CS=0x6C101800;//US Synchronization​Channel
const u32 B00181080S=0x80101800;//CS Beat​Rejection​Flag
const u32 B00181085S=0x85101800;//LO PVC​Rejection
const u32 B00181086S=0x86101800;//IS Skip​Beats
const u32 B00181088S=0x88101800;//IS Heart​Rate
const u32 B00181100S=0x00111800;//DS Reconstruction​Diameter
const u32 B00181120S=0x20111800;//DS Gantry​Detector​Tilt
const u32 B00181121S=0x21111800;//DS Gantry​Detector​Slew
const u32 B00181147S=0x47111800;//CS Field​Of​View​Shape
const u32 B00181149S=0x49111800;//IS Field​Of​View​Dimensions
const u32 B00181160S=0x60111800;//SH Filter​Type
const u32 B00181180S=0x80111800;//SH Collimator​Grid​Name
const u32 B00181181S=0x81111800;//CS Collimator​Type
const u32 B00181190S=0x90111800;//DS Focal​Spots
const u32 B00181200S=0x00121800;//DA Date​Of​Last​Calibration
const u32 B00181201S=0x01121800;//TM Time​Of​Last​Calibration
const u32 B00181204S=0x04121800;//DA Date​Of​Manufacture
const u32 B00181205S=0x05121800;//DA Date​Of​Installation
const u32 B00181210S=0x10121800;//SH Convolution​Kernel
const u32 B00181260S=0x60121800;//SH Plate​Type
const u32 B00181261S=0x61121800;//LO Phosphor​Type
const u32 B00181800S=0x00181800;//CS Acquisition​Time​Synchronized
const u32 B00181801S=0x01181800;//SH Time​Source
const u32 B00181802S=0x02181800;//CS Time​Distribution​Protocol
const u32 B00181803S=0x03181800;//LO NTP​Source​Address
const u32 B00185100S=0x00511800;//CS Patient​Position
const u32 B00185101S=0x01511800;//CS View​Position
const u32 B0020000ES=0x0E002000;//UI Series​Instance​UID
const u32 B00200011S=0x11002000;//IS Series​Number
const u32 B00200052S=0x52002000;//UI Frame​Of​Reference​UID
const u32 B00200060S=0x60002000;//CS Laterality
const u32 B00200200S=0x00022000;//UI Synchronization​Frame​Of​Reference​UID
const u32 B00201040S=0x40102000;//LO Position​Reference​Indicator
const u32 B00209307S=0x07932000;//CS Ultrasound​Acquisition​Geometry
const u32 B00209308S=0x08932000;//FD Apex​Position
const u32 B00209309S=0x09932000;//FD Volume​To​Transducer​Mapping​Matrix
const u32 B0020930AS=0x0A932000;//FD Volume​To​Table​Mapping​Matrix
const u32 B0020930BS=0x0B932000;//CS Volume​To​Transducer​Relationship
const u32 B0020930CS=0x0C932000;//CS Patient​Frame​Of​Reference​Source
const u32 B00209312S=0x12932000;//UI Volume​Frame​Of​Reference​UID
const u32 B00209313S=0x13932000;//UI Table​Frame​Of​Reference​UID
const u32 B00280051S=0x51002800;//CS Corrected​Image
const u32 B00280108S=0x08012800;//US or SS Smallest​Pixel​Value​In​Series
const u32 B00280109S=0x09012800;//US or SS Largest​Pixel​Value​In​Series
const u32 B00280120S=0x20012800;//US or SS Pixel​Padding​Value
const u32 B00400244S=0x44024000;//DA Performed​Procedure​Step​Start​Date
const u32 B00400245S=0x45024000;//TM Performed​Procedure​Step​Start​Time
const u32 B00400250S=0x50024000;//DA Performed​Procedure​Step​End​Date
const u32 B00400251S=0x51024000;//TM Performed​Procedure​Step​End​Time
const u32 B00400253S=0x53024000;//SH Performed​Procedure​Step​ID
const u32 B00400254S=0x54024000;//LO Performed​Procedure​Step​Description
const u32 B00400260S=0x60024000;//SQ Performed​Protocol​Code​Sequence
const u32 B00400261S=0x61024000;//CS Performed​Protocol​Type
const u32 B00400275S=0x75024000;//SQ Request​Attributes​Sequence
const u32 B00400280S=0x80024000;//ST Comments​On​The​Performed​Procedure​Step
*/
const u32 L00080021S=0x00080021;//DA Series​Date
const u32 L00080031S=0x00080031;//TM Series​Time
const u32 L00080060S=0x00080060;//CS Modality
const u32 L00080064S=0x00080064;//CS Conversion​Type
const u32 L00080068S=0x00080068;//CS Presentation​Intent​Type
const u32 L00080070S=0x00080070;//LO Manufacturer
const u32 L00080080S=0x00080080;//LO Institution​Name
const u32 L00080081S=0x00080081;//ST Institution​Address
const u32 L00081010S=0x00081010;//SH Station​Name
const u32 L0008103ES=0x0008103E;//LO Series​Description
const u32 L0008103FS=0x0008103F;//SQ Series​Description​Code​Sequence
const u32 L00081040S=0x00081040;//LO Institutional​Department​Name
const u32 L00081041S=0x00081041;//SQ Institutional​Department​Type​Code​Sequence
const u32 L00081050S=0x00081050;//PN Performing​Physician​Name
const u32 L00081052S=0x00081052;//SQ Performing​Physician​Identification​Sequence
const u32 L00081070S=0x00081070;//PN Operators​Name
const u32 L00081072S=0x00081072;//SQ Operator​Identification​Sequence
const u32 L00081090S=0x00081090;//LO Manufacturer​Model​Name
const u32 L00081111S=0x00081111;//SQ Referenced​Performed​Procedure​Step​Sequence
const u32 L00081250S=0x00081250;//SQ Related​Series​Sequence
const u32 L00120060S=0x00120060;//LO Clinical​Trial​Coordinating​Center​Name
const u32 L00120071S=0x00120071;//LO Clinical​Trial​Series​ID
const u32 L00120072S=0x00120072;//LO Clinical​Trial​Series​Description
const u32 L0016004DS=0x0016004D;//UT Camera​Owner​Name
const u32 L0016004ES=0x0016004E;//DS Lens​Specification
const u32 L0016004FS=0x0016004F;//UT Lens​Make
const u32 L00160050S=0x00160050;//UT Lens​Model
const u32 L00160051S=0x00160051;//UT Lens​Serial​Number
const u32 L00180015S=0x00180015;//CS Body​Part​Examined
const u32 L00180026S=0x00180026;//SQ Intervention​Drug​Information​Sequence
const u32 L00180071S=0x00180071;//CS Acquisition​Termination​Condition
const u32 L00180073S=0x00180073;//CS Acquisition​Start​Condition
const u32 L00180074S=0x00180074;//IS Acquisition​Start​Condition​Data
const u32 L00180075S=0x00180075;//IS Acquisition​Termination​Condition​Data
const u32 L00181000S=0x00181000;//LO Device​Serial​Number
const u32 L00181002S=0x00181002;//UI Device​UID
const u32 L00181008S=0x00181008;//LO Gantry​ID
const u32 L0018100AS=0x0018100A;//SQ UDI​Sequence
const u32 L0018100BS=0x0018100B;//UI Manufacturer​Device​Class​UID
const u32 L00181010S=0x00181010;//LO Secondary​Capture​Device​ID
const u32 L00181016S=0x00181016;//LO Secondary​Capture​Device​Manufacturer
const u32 L00181018S=0x00181018;//LO Secondary​Capture​Device​Manufacturer​Model​Name
const u32 L00181019S=0x00181019;//LO Secondary​Capture​Device​Software​Versions
const u32 L00181020S=0x00181020;//LO Software​Versions
const u32 L00181022S=0x00181022;//SH Video​Image​Format​Acquired
const u32 L00181023S=0x00181023;//LO Digital​Image​Format​Acquired
const u32 L00181030S=0x00181030;//LO Protocol​Name
const u32 L00181050S=0x00181050;//DS Spatial​Resolution
const u32 L00181061S=0x00181061;//LO Trigger​Source​Or​Type
const u32 L00181064S=0x00181064;//LO Cardiac​Framing​Type
const u32 L0018106AS=0x0018106A;//CS Synchronization​Trigger
const u32 L0018106CS=0x0018106C;//US Synchronization​Channel
const u32 L00181080S=0x00181080;//CS Beat​Rejection​Flag
const u32 L00181085S=0x00181085;//LO PVC​Rejection
const u32 L00181086S=0x00181086;//IS Skip​Beats
const u32 L00181088S=0x00181088;//IS Heart​Rate
const u32 L00181100S=0x00181100;//DS Reconstruction​Diameter
const u32 L00181120S=0x00181120;//DS Gantry​Detector​Tilt
const u32 L00181121S=0x00181121;//DS Gantry​Detector​Slew
const u32 L00181147S=0x00181147;//CS Field​Of​View​Shape
const u32 L00181149S=0x00181149;//IS Field​Of​View​Dimensions
const u32 L00181160S=0x00181160;//SH Filter​Type
const u32 L00181180S=0x00181180;//SH Collimator​Grid​Name
const u32 L00181181S=0x00181181;//CS Collimator​Type
const u32 L00181190S=0x00181190;//DS Focal​Spots
const u32 L00181200S=0x00181200;//DA Date​Of​Last​Calibration
const u32 L00181201S=0x00181201;//TM Time​Of​Last​Calibration
const u32 L00181204S=0x00181204;//DA Date​Of​Manufacture
const u32 L00181205S=0x00181205;//DA Date​Of​Installation
const u32 L00181210S=0x00181210;//SH Convolution​Kernel
const u32 L00181260S=0x00181260;//SH Plate​Type
const u32 L00181261S=0x00181261;//LO Phosphor​Type
const u32 L00181800S=0x00181800;//CS Acquisition​Time​Synchronized
const u32 L00181801S=0x00181801;//SH Time​Source
const u32 L00181802S=0x00181802;//CS Time​Distribution​Protocol
const u32 L00181803S=0x00181803;//LO NTP​Source​Address
const u32 L00185100S=0x00185100;//CS Patient​Position
const u32 L00185101S=0x00185101;//CS View​Position
const u32 L0020000ES=0x0020000E;//UI Series​Instance​UID
const u32 L00200011S=0x00200011;//IS Series​Number
const u32 L00200052S=0x00200052;//UI Frame​Of​Reference​UID
const u32 L00200060S=0x00200060;//CS Laterality
const u32 L00200200S=0x00200200;//UI Synchronization​Frame​Of​Reference​UID
const u32 L00201040S=0x00201040;//LO Position​Reference​Indicator
const u32 L00209307S=0x00209307;//CS Ultrasound​Acquisition​Geometry
const u32 L00209308S=0x00209308;//FD Apex​Position
const u32 L00209309S=0x00209309;//FD Volume​To​Transducer​Mapping​Matrix
const u32 L0020930AS=0x0020930A;//FD Volume​To​Table​Mapping​Matrix
const u32 L0020930BS=0x0020930B;//CS Volume​To​Transducer​Relationship
const u32 L0020930CS=0x0020930C;//CS Patient​Frame​Of​Reference​Source
const u32 L00209312S=0x00209312;//UI Volume​Frame​Of​Reference​UID
const u32 L00209313S=0x00209313;//UI Table​Frame​Of​Reference​UID
const u32 L00280051S=0x00280051;//CS Corrected​Image
const u32 L00280108S=0x00280108;//US or SS Smallest​Pixel​Value​In​Series
const u32 L00280109S=0x00280109;//US or SS Largest​Pixel​Value​In​Series
const u32 L00280120S=0x00280120;//US or SS Pixel​Padding​Value
const u32 L00400244S=0x00400244;//DA Performed​Procedure​Step​Start​Date
const u32 L00400245S=0x00400245;//TM Performed​Procedure​Step​Start​Time
const u32 L00400250S=0x00400250;//DA Performed​Procedure​Step​End​Date
const u32 L00400251S=0x00400251;//TM Performed​Procedure​Step​End​Time
const u32 L00400253S=0x00400253;//SH Performed​Procedure​Step​ID
const u32 L00400254S=0x00400254;//LO Performed​Procedure​Step​Description
const u32 L00400260S=0x00400260;//SQ Performed​Protocol​Code​Sequence
const u32 L00400261S=0x00400261;//CS Performed​Protocol​Type
const u32 L00400275S=0x00400275;//SQ Request​Attributes​Sequence
const u32 L00400280S=0x00400280;//ST Comments​On​The​Performed​Procedure​Step


#pragma mark base dataset series modality specific
/*
const u32 B00540013X=0x13005400;//SQ Energy​Window​Range​Sequence
const u32 B00540014X=0x14005400;//DS Energy​Window​Lower​Limit
const u32 B00540015X=0x15005400;//DS Energy​Window​Upper​Limit
const u32 B00540016X=0x16005400;//SQ Radio​pharmaceutical​Information​Sequence
const u32 B00540061X=0x61005400;//US Number​Of​RR​Intervals
const u32 B00540071X=0x71005400;//US Number​Of​Time​Slots
const u32 B00540081X=0x81005400;//US Number​Of​Slices
const u32 B00540101X=0x01015400;//US Number​Of​Time​Slices
const u32 B00540202X=0x02025400;//CS Type​Of​Detector​Motion
const u32 B00540410X=0x10045400;//SQ Patient​Orientation​Code​Sequence
const u32 B00540414X=0x14045400;//SQ Patient​Gantry​Relationship​Code​Sequence
const u32 B00540501X=0x01055400;//CS Scan​Progression​Direction
const u32 B00541000X=0x00105400;//CS Series​Type
const u32 B00541001X=0x01105400;//CS Units
const u32 B00541002X=0x02105400;//CS Counts​Source
const u32 B00541004X=0x04105400;//CS Reprojection​Method
const u32 B00541006X=0x06105400;//CS SUV​Type
const u32 B00541100X=0x00115400;//CS Randoms​Correction​Method
const u32 B00541101X=0x01115400;//LO Attenuation​Correction​Method
const u32 B00541102X=0x02115400;//CS Decay​Correction
const u32 B00541103X=0x03115400;//LO Reconstruction​Method
const u32 B00541104X=0x04115400;//LO Detector​Lines​Of​Response​Used
const u32 B00541105X=0x05115400;//LO Scatter​Correction​Method
const u32 B00541200X=0x00125400;//DS Axial​Acceptance
const u32 B00541201X=0x01125400;//IS Axial​Mash
const u32 B00541202X=0x02125400;//IS Transverse​Mash
const u32 B00541203X=0x03125400;//DS Detector​Element​Size
const u32 B00541210X=0x10125400;//DS Coincidence​Window​Width
const u32 B00541220X=0x20125400;//CS Secondary​Counts​Type
const u32 B00800013X=0x13008000;//SQ Referenced​Surface​Data​Sequence
const u32 B300A0700X=0x00070A30;//UI Treatment​Session​UID
*/
const u32 L00540013X=0x00540013;//SQ Energy​Window​Range​Sequence
const u32 L00540014X=0x00540014;//DS Energy​Window​Lower​Limit
const u32 L00540015X=0x00540015;//DS Energy​Window​Upper​Limit
const u32 L00540016X=0x00540016;//SQ Radio​pharmaceutical​Information​Sequence
const u32 L00540061X=0x00540061;//US Number​Of​RR​Intervals
const u32 L00540071X=0x00540071;//US Number​Of​Time​Slots
const u32 L00540081X=0x00540081;//US Number​Of​Slices
const u32 L00540101X=0x00540101;//US Number​Of​Time​Slices
const u32 L00540202X=0x00540202;//CS Type​Of​Detector​Motion
const u32 L00540410X=0x00540410;//SQ Patient​Orientation​Code​Sequence
const u32 L00540414X=0x00540414;//SQ Patient​Gantry​Relationship​Code​Sequence
const u32 L00540501X=0x00540501;//CS Scan​Progression​Direction
const u32 L00541000X=0x00541000;//CS Series​Type
const u32 L00541001X=0x00541001;//CS Units
const u32 L00541002X=0x00541002;//CS Counts​Source
const u32 L00541004X=0x00541004;//CS Reprojection​Method
const u32 L00541006X=0x00541006;//CS SUV​Type
const u32 L00541100X=0x00541100;//CS Randoms​Correction​Method
const u32 L00541101X=0x00541101;//LO Attenuation​Correction​Method
const u32 L00541102X=0x00541102;//CS Decay​Correction
const u32 L00541103X=0x00541103;//LO Reconstruction​Method
const u32 L00541104X=0x00541104;//LO Detector​Lines​Of​Response​Used
const u32 L00541105X=0x00541105;//LO Scatter​Correction​Method
const u32 L00541200X=0x00541200;//DS Axial​Acceptance
const u32 L00541201X=0x00541201;//IS Axial​Mash
const u32 L00541202X=0x00541202;//IS Transverse​Mash
const u32 L00541203X=0x00541203;//DS Detector​Element​Size
const u32 L00541210X=0x00541210;//DS Coincidence​Window​Width
const u32 L00541220X=0x20125400;//CS Secondary​Counts​Type
const u32 L00800013X=0x00800013;//SQ Referenced​Surface​Data​Sequence
const u32 L300A0700X=0x300A0700;//UI Treatment​Session​UID


const u32 EStag[]={
   L00080020E,//DA Study​Date
   L00080021S,//DA Series​Date
   L00080030E,//TM Study​Time
   L00080031S,//TM Series​Time
   L00080050E,//SH Accession​Number
   L00080051E,//SQ Issuer​Of​Accession​Number​Sequence
   L00080060S,//CS Modality
   L00080064S,//CS Conversion​Type
   L00080068S,//CS Presentation​Intent​Type
   L00080070S,//LO Manufacturer
   L00080080S,//LO Institution​Name
   L00080081S,//ST Institution​Address
   L00080090E,//PN Referring​Physician​Name
   L00080096E,//SQ Referring​Physician​Identification​Sequence
   L0008009CE,//PN Consulting​Physician​Name
   L0008009DE,//SQ Consulting​Physician​Identification​Sequence
   L00081010S,//SH Station​Name
   L00081030E,//LO Study​Description
   L00081032E,//SQ Procedure​Code​Sequence   L0008103ES,//LO Series​Description
   L0008103FS,//SQ Series​Description​Code​Sequence
   L00081040S,//LO Institutional​Department​Name
   L00081041S,//SQ Institutional​Department​Type​Code​Sequence
   L00081048E,//PN Physicians​Of​Record
   L00081049E,//SQ Physicians​Of​Record​Identification​Sequence
   L00081050S,//PN Performing​Physician​Name
   L00081052S,//SQ Performing​Physician​Identification​Sequence
   L00081060E,//PN Name​Of​Physicians​Reading​Study
   L00081062E,//SQ Physicians​Reading​Study​Identification​Sequence
   L00081070S,//PN Operators​Name
   L00081072S,//SQ Operator​Identification​Sequence
   L00081080E,//LO Admitting​Diagnoses​Description
   L00081084E,//SQ Admitting​Diagnoses​Code​Sequence
   L00081090S,//LO Manufacturer​Model​Name
   L00081110E,//SQ Referenced​Study​Sequence
   L00081111S,//SQ Referenced​Performed​Procedure​Step​Sequence
   L00081120P,//SQ Referenced Patient Sequence
   L00081250S,//SQ Related​Series​Sequence
   L00100010P,//PN Patient​Name
   L00100020P,//LO Patient​ID
   L00100021P,//LO Issuer​Of​Patient​ID
   L00100022P,//CS Type​Of​Patient​ID
   L00100024P,//SQ Issuer​Of​Patient​ID​Qualifiers​Sequence
   L00100026P,//SQ Source​Patient​Group​Identification​Sequence
   L00100027P,//SQ Group​Of​Patients​Identification​Sequence
   L00100030P,//DA Patient​Birth​Date
   L00100032P,//TM Patient​Birth​Time
   L00100033P,//LO Patient​Birth​Date​In​Alternative​Calendar
   L00100034P,//LO Patient​Death​Date​In​Alternative​Calendar
   L00100035P,//CS Patient​Alternative​Calendar
   L00100040P,//CS Patient​Sex
   L00100200P,//CS Quality​Control​Subject
   L00100212P,//UC Strain​Description
   L00100213P,//LO Strain​Nomenclature
   L00100216P,//SQ Strain​Stock​Sequence
   L00100218P,//UT Strain​Additional​Information
   L00100219P,//SQ Strain​Code​Sequence
   L00100221P,//SQ Genetic​Modifications​​Sequence
   L00101000P,//LO Other​Patient​IDs
   L00101001P,//PN Other​Patient​Names
   L00101002P,//SQ Other​Patient​IDs​Sequence
   L00101010E,//AS Patient​Age
   L00101020E,//DS Patient​Size
   L00101021E,//SQ Patient​Size​Code​Sequence
   L00101022E,//DS Patient​Body​Mass​Index
   L00101023E,//DS Measured​APDimension
   L00101024E,//DS Measured​Lateral​Dimension
   L00101030E,//DS Patient​Weight
   L00101100P,//SQ Referenced​Patient​Photo​Sequence
   L00102000E,//LO Medical​Alerts
   L00102110E,//LO Allergies
   L00102160P,//SH Ethnic​Group
   L00102180E,//SH Occupation
   L001021A0E,//CS Smoking​Status
   L001021B0E,//LT Additional​Patient​History
   L001021C0E,//US Pregnancy​Status
   L001021D0E,//DA Last​Menstrual​Date
   L00102201P,//LO Patient​Species​Description
   L00102202P,//SQ Patient​Species​Code​Sequence
   L00102203E,//CS Patient​Sex​Neutered
   L00102292P,//LO Patient​Breed​Description
   L00102293P,//SQ Patient​Breed​Code​Sequence
   L00102294P,//SQ Breed​Registration​Sequence
   L00102297P,//PN Responsible​Person
   L00102298P,//CS Responsible​Person​Role
   L00102299P,//LO Responsible​Organization
   L00104000P,//LT Patient​Comments
   L00120010P,//LO Clinical​Trial​Sponsor​Name
   L00120020P,//LO Clinical​Trial​Protocol​ID
   L00120021P,//LO Clinical​Trial​Protocol​Name
   L00120030P,//LO Clinical​Trial​Site​ID
   L00120031P,//LO Clinical​Trial​Site​Name
   L00120040P,//LO Clinical​Trial​Subject​ID
   L00120042P,//LO Clinical​Trial​Subject​Reading​ID
   L00120050E,//LO Clinical​Trial​Time​Point​ID
   L00120051E,//ST Clinical​Trial​Time​Point​Description
   L00120052E,//FD Longitudinal​Temporal​Offset​From​Event
   L00120053E,//CS Longitudinal​Temporal​Event​Type
   L00120054E,//SQ Clinical​Trial​Time​Point​Type​Code​Sequence
   L00120060S,//LO Clinical​Trial​Coordinating​Center​Name
   L00120062P,//CS Patient​Identity​Removed
   L00120063P,//LO Deidentification​Method
   L00120064P,//SQ Deidentification​Method​Code​Sequence
   L00120071S,//LO Clinical​Trial​Series​ID
   L00120072S,//LO Clinical​Trial​Series​Description
   L00120081P,//LO Clinical​Trial​Protocol​Ethics​Committee​Name
   L00120082P,//LO Clinical​Trial​Protocol​Ethics​Committee​Approval​Number
   L00120083E,//SQ Consent​For​Clinical​Trial​Use​Sequence
   L0016004DS,//UT Camera​Owner​Name
   L0016004ES,//DS Lens​Specification
   L0016004FS,//UT Lens​Make
   L00160050S,//UT Lens​Model
   L00160051S,//UT Lens​Serial​Number
   L00180015S,//CS Body​Part​Examined
   L00180026S,//SQ Intervention​Drug​Information​Sequence
   L00180071S,//CS Acquisition​Termination​Condition
   L00180073S,//CS Acquisition​Start​Condition
   L00180074S,//IS Acquisition​Start​Condition​Data
   L00180075S,//IS Acquisition​Termination​Condition​Data
   L00181000S,//LO Device​Serial​Number
   L00181002S,//UI Device​UID
   L00181008S,//LO Gantry​ID
   L0018100AS,//SQ UDI​Sequence
   L0018100BS,//UI Manufacturer​Device​Class​UID
   L00181010S,//LO Secondary​Capture​Device​ID
   L00181016S,//LO Secondary​Capture​Device​Manufacturer
   L00181018S,//LO Secondary​Capture​Device​Manufacturer​Model​Name
   L00181019S,//LO Secondary​Capture​Device​Software​Versions
   L00181020S,//LO Software​Versions
   L00181022S,//SH Video​Image​Format​Acquired
   L00181023S,//LO Digital​Image​Format​Acquired
   L00181030S,//LO Protocol​Name
   L00181050S,//DS Spatial​Resolution
   L00181061S,//LO Trigger​Source​Or​Type
   L00181064S,//LO Cardiac​Framing​Type
   L0018106AS,//CS Synchronization​Trigger
   L0018106CS,//US Synchronization​Channel
   L00181080S,//CS Beat​Rejection​Flag
   L00181085S,//LO PVC​Rejection
   L00181086S,//IS Skip​Beats
   L00181088S,//IS Heart​Rate
   L00181100S,//DS Reconstruction​Diameter
   L00181120S,//DS Gantry​Detector​Tilt
   L00181121S,//DS Gantry​Detector​Slew
   L00181147S,//CS Field​Of​View​Shape
   L00181149S,//IS Field​Of​View​Dimensions
   L00181160S,//SH Filter​Type
   L00181180S,//SH Collimator​Grid​Name
   L00181181S,//CS Collimator​Type
   L00181190S,//DS Focal​Spots
   L00181200S,//DA Date​Of​Last​Calibration
   L00181201S,//TM Time​Of​Last​Calibration
   L00181204S,//DA Date​Of​Manufacture
   L00181205S,//DA Date​Of​Installation
   L00181210S,//SH Convolution​Kernel
   L00181260S,//SH Plate​Type
   L00181261S,//LO Phosphor​Type
   L00181800S,//CS Acquisition​Time​Synchronized
   L00181801S,//SH Time​Source
   L00181802S,//CS Time​Distribution​Protocol
   L00181803S,//LO NTP​Source​Address
   L00185100S,//CS Patient​Position
   L00185101S,//CS View​Position
   L0020000DE,//UI Study​Instance​UID
   L0020000ES,//UI Series​Instance​UID
   L00200010E,//SH Study​ID
   L00200011S,//IS Series​Number
   L00200052S,//UI Frame​Of​Reference​UID
   L00200060S,//CS Laterality
   L00200200S,//UI Synchronization​Frame​Of​Reference​UID
   L00201040S,//LO Position​Reference​Indicator
   L00209307S,//CS Ultrasound​Acquisition​Geometry
   L00209308S,//FD Apex​Position
   L00209309S,//FD Volume​To​Transducer​Mapping​Matrix
   L0020930AS,//FD Volume​To​Table​Mapping​Matrix
   L0020930BS,//CS Volume​To​Transducer​Relationship
   L0020930CS,//CS Patient​Frame​Of​Reference​Source
   L00209312S,//UI Volume​Frame​Of​Reference​UID
   L00209313S,//UI Table​Frame​Of​Reference​UID
   L00280051S,//CS Corrected​Image
   L00280108S,//US or SS Smallest​Pixel​Value​In​Series
   L00280109S,//US or SS Largest​Pixel​Value​In​Series
   L00280120S,//US or SS Pixel​Padding​Value
   L00321033E,//LO Requesting​Service
   L00321034E,//SQ Requesting​Service​Code​Sequence
   L00321066E,//UT Reason​For​Visit
   L00321067E,//SQ Reason​For​Visit​Code​Sequence
   L00380010E,//LO Admission​ID
   L00380014E,//SQ Issuer​Of​Admission​ID​Sequence
   L00380060E,//LO Service​Episode​ID
   L00380062E,//LO Service​Episode​Description
   L00380064E,//SQ Issuer​Of​Service​Episode​ID​Sequence
   L00385000E,//LO Patient​State
   L00400244S,//DA Performed​Procedure​Step​Start​Date
   L00400245S,//TM Performed​Procedure​Step​Start​Time
   L00400250S,//DA Performed​Procedure​Step​End​Date
   L00400251S,//TM Performed​Procedure​Step​End​Time
   L00400253S,//SH Performed​Procedure​Step​ID
   L00400254S,//LO Performed​Procedure​Step​Description
   L00400260S,//SQ Performed​Protocol​Code​Sequence
   L00400261S,//CS Performed​Protocol​Type
   L00400275S,//SQ Request​Attributes​Sequence
   L00400280S,//ST Comments​On​The​Performed​Procedure​Step
   L00401012E,//SQ Reason​For​Performed​Procedure​Code​Sequence
   L00540013X,//SQ Energy​Window​Range​Sequence
   L00540014X,//DS Energy​Window​Lower​Limit
   L00540015X,//DS Energy​Window​Upper​Limit
   L00540016X,//SQ Radio​pharmaceutical​Information​Sequence
   L00540061X,//US Number​Of​RR​Intervals
   L00540071X,//US Number​Of​Time​Slots
   L00540081X,//US Number​Of​Slices
   L00540101X,//US Number​Of​Time​Slices
   L00540202X,//CS Type​Of​Detector​Motion
   L00540410X,//SQ Patient​Orientation​Code​Sequence
   L00540414X,//SQ Patient​Gantry​Relationship​Code​Sequence
   L00540501X,//CS Scan​Progression​Direction
   L00541000X,//CS Series​Type
   L00541001X,//CS Units
   L00541002X,//CS Counts​Source
   L00541004X,//CS Reprojection​Method
   L00541006X,//CS SUV​Type
   L00541100X,//CS Randoms​Correction​Method
   L00541101X,//LO Attenuation​Correction​Method
   L00541102X,//CS Decay​Correction
   L00541103X,//LO Reconstruction​Method
   L00541104X,//LO Detector​Lines​Of​Response​Used
   L00541105X,//LO Scatter​Correction​Method
   L00541200X,//DS Axial​Acceptance
   L00541201X,//IS Axial​Mash
   L00541202X,//IS Transverse​Mash
   L00541203X,//DS Detector​Element​Size
   L00541210X,//DS Coincidence​Window​Width
   L00541220X,//CS Secondary​Counts​Type
   L00800013X,//SQ Referenced​Surface​Data​Sequence
   L300A0700X //UI Treatment​Session​UID
};

const u8 P=0x00;//patient level
const u8 C=0x00;//clinical study level
const u8 S=0x01;//series level
const u8 X=0x01;//special series level
const u8 EStype[]={
   C,//DA Study​Date
   S,//DA Series​Date
   C,//TM Study​Time
   S,//TM Series​Time
   C,//SH Accession​Number
   C,//SQ Issuer​Of​Accession​Number​Sequence
   S,//CS Modality
   S,//CS Conversion​Type
   S,//CS Presentation​Intent​Type
   S,//LO Manufacturer
   S,//LO Institution​Name
   S,//ST Institution​Address
   C,//PN Referring​Physician​Name
   C,//SQ Referring​Physician​Identification​Sequence
   C,//PN Consulting​Physician​Name
   C,//SQ Consulting​Physician​Identification​Sequence
   S,//SH Station​Name
   C,//LO Study​Description
   C,//SQ Procedure​Code​Sequence   L0008103ES,//LO Series​Description
   S,//SQ Series​Description​Code​Sequence
   S,//LO Institutional​Department​Name
   S,//SQ Institutional​Department​Type​Code​Sequence
   C,//PN Physicians​Of​Record
   C,//SQ Physicians​Of​Record​Identification​Sequence
   S,//PN Performing​Physician​Name
   S,//SQ Performing​Physician​Identification​Sequence
   C,//PN Name​Of​Physicians​Reading​Study
   C,//SQ Physicians​Reading​Study​Identification​Sequence
   S,//PN Operators​Name
   S,//SQ Operator​Identification​Sequence
   C,//LO Admitting​Diagnoses​Description
   C,//SQ Admitting​Diagnoses​Code​Sequence
   S,//LO Manufacturer​Model​Name
   C,//SQ Referenced​Study​Sequence
   S,//SQ Referenced​Performed​Procedure​Step​Sequence
   P,//SQ Referenced Patient Sequence
   S,//SQ Related​Series​Sequence
   P,//PN Patient​Name
   P,//LO Patient​ID
   P,//LO Issuer​Of​Patient​ID
   P,//CS Type​Of​Patient​ID
   P,//SQ Issuer​Of​Patient​ID​Qualifiers​Sequence
   P,//SQ Source​Patient​Group​Identification​Sequence
   P,//SQ Group​Of​Patients​Identification​Sequence
   P,//DA Patient​Birth​Date
   P,//TM Patient​Birth​Time
   P,//LO Patient​Birth​Date​In​Alternative​Calendar
   P,//LO Patient​Death​Date​In​Alternative​Calendar
   P,//CS Patient​Alternative​Calendar
   P,//CS Patient​Sex
   P,//CS Quality​Control​Subject
   P,//UC Strain​Description
   P,//LO Strain​Nomenclature
   P,//SQ Strain​Stock​Sequence
   P,//UT Strain​Additional​Information
   P,//SQ Strain​Code​Sequence
   P,//SQ Genetic​Modifications​​Sequence
   P,//LO Other​Patient​IDs
   P,//PN Other​Patient​Names
   P,//SQ Other​Patient​IDs​Sequence
   C,//AS Patient​Age
   C,//DS Patient​Size
   C,//SQ Patient​Size​Code​Sequence
   C,//DS Patient​Body​Mass​Index
   C,//DS Measured​APDimension
   C,//DS Measured​Lateral​Dimension
   C,//DS Patient​Weight
   P,//SQ Referenced​Patient​Photo​Sequence
   C,//LO Medical​Alerts
   C,//LO Allergies
   P,//SH Ethnic​Group
   C,//SH Occupation
   C,//CS Smoking​Status
   C,//LT Additional​Patient​History
   C,//US Pregnancy​Status
   C,//DA Last​Menstrual​Date
   P,//LO Patient​Species​Description
   P,//SQ Patient​Species​Code​Sequence
   C,//CS Patient​Sex​Neutered
   P,//LO Patient​Breed​Description
   P,//SQ Patient​Breed​Code​Sequence
   P,//SQ Breed​Registration​Sequence
   P,//PN Responsible​Person
   P,//CS Responsible​Person​Role
   P,//LO Responsible​Organization
   P,//LT Patient​Comments
   P,//LO Clinical​Trial​Sponsor​Name
   P,//LO Clinical​Trial​Protocol​ID
   P,//LO Clinical​Trial​Protocol​Name
   P,//LO Clinical​Trial​Site​ID
   P,//LO Clinical​Trial​Site​Name
   P,//LO Clinical​Trial​Subject​ID
   P,//LO Clinical​Trial​Subject​Reading​ID
   C,//LO Clinical​Trial​Time​Point​ID
   C,//ST Clinical​Trial​Time​Point​Description
   C,//FD Longitudinal​Temporal​Offset​From​Event
   C,//CS Longitudinal​Temporal​Event​Type
   C,//SQ Clinical​Trial​Time​Point​Type​Code​Sequence
   S,//LO Clinical​Trial​Coordinating​Center​Name
   P,//CS Patient​Identity​Removed
   P,//LO Deidentification​Method
   P,//SQ Deidentification​Method​Code​Sequence
   S,//LO Clinical​Trial​Series​ID
   S,//LO Clinical​Trial​Series​Description
   P,//LO Clinical​Trial​Protocol​Ethics​Committee​Name
   P,//LO Clinical​Trial​Protocol​Ethics​Committee​Approval​Number
   C,//SQ Consent​For​Clinical​Trial​Use​Sequence
   S,//UT Camera​Owner​Name
   S,//DS Lens​Specification
   S,//UT Lens​Make
   S,//UT Lens​Model
   S,//UT Lens​Serial​Number
   S,//CS Body​Part​Examined
   S,//SQ Intervention​Drug​Information​Sequence
   S,//CS Acquisition​Termination​Condition
   S,//CS Acquisition​Start​Condition
   S,//IS Acquisition​Start​Condition​Data
   S,//IS Acquisition​Termination​Condition​Data
   S,//LO Device​Serial​Number
   S,//UI Device​UID
   S,//LO Gantry​ID
   S,//SQ UDI​Sequence
   S,//UI Manufacturer​Device​Class​UID
   S,//LO Secondary​Capture​Device​ID
   S,//LO Secondary​Capture​Device​Manufacturer
   S,//LO Secondary​Capture​Device​Manufacturer​Model​Name
   S,//LO Secondary​Capture​Device​Software​Versions
   S,//LO Software​Versions
   S,//SH Video​Image​Format​Acquired
   S,//LO Digital​Image​Format​Acquired
   S,//LO Protocol​Name
   S,//DS Spatial​Resolution
   S,//LO Trigger​Source​Or​Type
   S,//LO Cardiac​Framing​Type
   S,//CS Synchronization​Trigger
   S,//US Synchronization​Channel
   S,//CS Beat​Rejection​Flag
   S,//LO PVC​Rejection
   S,//IS Skip​Beats
   S,//IS Heart​Rate
   S,//DS Reconstruction​Diameter
   S,//DS Gantry​Detector​Tilt
   S,//DS Gantry​Detector​Slew
   S,//CS Field​Of​View​Shape
   S,//IS Field​Of​View​Dimensions
   S,//SH Filter​Type
   S,//SH Collimator​Grid​Name
   S,//CS Collimator​Type
   S,//DS Focal​Spots
   S,//DA Date​Of​Last​Calibration
   S,//TM Time​Of​Last​Calibration
   S,//DA Date​Of​Manufacture
   S,//DA Date​Of​Installation
   S,//SH Convolution​Kernel
   S,//SH Plate​Type
   S,//LO Phosphor​Type
   S,//CS Acquisition​Time​Synchronized
   S,//SH Time​Source
   S,//CS Time​Distribution​Protocol
   S,//LO NTP​Source​Address
   S,//CS Patient​Position
   S,//CS View​Position
   C,//UI Study​Instance​UID
   S,//UI Series​Instance​UID
   C,//SH Study​ID
   S,//IS Series​Number
   S,//UI Frame​Of​Reference​UID
   S,//CS Laterality
   S,//UI Synchronization​Frame​Of​Reference​UID
   S,//LO Position​Reference​Indicator
   S,//CS Ultrasound​Acquisition​Geometry
   S,//FD Apex​Position
   S,//FD Volume​To​Transducer​Mapping​Matrix
   S,//FD Volume​To​Table​Mapping​Matrix
   S,//CS Volume​To​Transducer​Relationship
   S,//CS Patient​Frame​Of​Reference​Source
   S,//UI Volume​Frame​Of​Reference​UID
   S,//UI Table​Frame​Of​Reference​UID
   S,//CS Corrected​Image
   S,//US or SS Smallest​Pixel​Value​In​Series
   S,//US or SS Largest​Pixel​Value​In​Series
   S,//US or SS Pixel​Padding​Value
   C,//LO Requesting​Service
   C,//SQ Requesting​Service​Code​Sequence
   C,//UT Reason​For​Visit
   C,//SQ Reason​For​Visit​Code​Sequence
   C,//LO Admission​ID
   C,//SQ Issuer​Of​Admission​ID​Sequence
   C,//LO Service​Episode​ID
   C,//LO Service​Episode​Description
   C,//SQ Issuer​Of​Service​Episode​ID​Sequence
   C,//LO Patient​State
   S,//DA Performed​Procedure​Step​Start​Date
   S,//TM Performed​Procedure​Step​Start​Time
   S,//DA Performed​Procedure​Step​End​Date
   S,//TM Performed​Procedure​Step​End​Time
   S,//SH Performed​Procedure​Step​ID
   S,//LO Performed​Procedure​Step​Description
   S,//SQ Performed​Protocol​Code​Sequence
   S,//CS Performed​Protocol​Type
   S,//SQ Request​Attributes​Sequence
   S,//ST Comments​On​The​Performed​Procedure​Step
   C,//SQ Reason​For​Performed​Procedure​Code​Sequence
   X,//SQ Energy​Window​Range​Sequence
   X,//DS Energy​Window​Lower​Limit
   X,//DS Energy​Window​Upper​Limit
   X,//SQ Radio​pharmaceutical​Information​Sequence
   X,//US Number​Of​RR​Intervals
   X,//US Number​Of​Time​Slots
   X,//US Number​Of​Slices
   X,//US Number​Of​Time​Slices
   X,//CS Type​Of​Detector​Motion
   X,//SQ Patient​Orientation​Code​Sequence
   X,//SQ Patient​Gantry​Relationship​Code​Sequence
   X,//CS Scan​Progression​Direction
   X,//CS Series​Type
   X,//CS Units
   X,//CS Counts​Source
   X,//CS Reprojection​Method
   X,//CS SUV​Type
   X,//CS Randoms​Correction​Method
   X,//LO Attenuation​Correction​Method
   X,//CS Decay​Correction
   X,//LO Reconstruction​Method
   X,//LO Detector​Lines​Of​Response​Used
   X,//LO Scatter​Correction​Method
   X,//DS Axial​Acceptance
   X,//IS Axial​Mash
   X,//IS Transverse​Mash
   X,//DS Detector​Element​Size
   X,//DS Coincidence​Window​Width
   X,//CS Secondary​Counts​Type
   X,//SQ Referenced​Surface​Data​Sequence
   X //UI Treatment​Session​UID
};


u64 edckvPrefix(u32 Btag,
                   u16 *ESidx,
                   u16 Bcccc,
                   u16 Bffff,
                   s16 Bssss,
                   s16 Brrrr,
                   s16 Biiii
                   )
{
   //https://github.com/jacquesfauquex/DCKV/wiki/eDCKV
   
   u32 Ltag=u32swap(Btag);   
   if (Ltag < EStag[*ESidx])
   {
      //instance level
      return
        0x0300
      + (Bssss*0x10000)
      + (Bffff*0x100000000)
      + (Biiii*0x1000000000000)
      ;
   }
   else
   {
      while ((Ltag > EStag[*ESidx]) && (*ESidx < 234)) (*ESidx)++;
      if (Ltag == EStag[*ESidx])
      {
         if (EStype[*ESidx]==0)//patient|study
         {
            return 0x0000000000000000;
         }
         else //series
         {
            return
              0x0100
            + (Bssss*0x10000)
            + (Bcccc*0x100000000)
            + (Brrrr*0x1000000000000)
            ;
         }
      }
      //instance level
      return
        0x0300
      + (Bssss*0x10000)
      + (Bffff*0x100000000)
      + (Biiii*0x1000000000000)
      ;
   }
      
   return 0xFFFFFFFFFFFFFFFF;//should not be here
}
