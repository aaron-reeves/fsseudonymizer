
#include "cbvdrecord.h"

#include <ar_general_purpose/strutils.h>
#include <ar_general_purpose/log.h>

#include <epic_general_purpose/cph.h>
#include <epic_general_purpose/cpostcode.h>
#include <epic_general_purpose/epicutils.h>

#include "cbvddata.h"

#include "cprocessor.h"

CBVDCoreFields globalFieldList;


// Regular expressions used in this file
//--------------------------------------
QRegExp reBioBestSampleResult( "^[<>]?\\s*[0-9]+$" );


// WHEN ADDING FIELDS OR FIELD ALIASES:
//=====================================
//  - Check buildGlobalFieldList()
//  - Check CBVDRecord::buildEncryptedField()
//  - Check CBVDCoreFields::fieldHeadersOK()

void buildGlobalFieldList() {
  // (Mostly) required core database fields.
  //----------------------------------------
  globalFieldList.addField( QStringLiteral("lab_test_reference"), QVariant::String, true, false );
  globalFieldList.addField( QStringLiteral("county"), QVariant::Int, true, false );
  globalFieldList.addField( QStringLiteral("parish"), QVariant::Int, true, false );
  globalFieldList.addField( QStringLiteral("valid_encrypted_cph"), QVariant::Bool, false, false ); // Optional in most files, but required for database population
  globalFieldList.addField( QStringLiteral("encrypted_cph"), QVariant::String, true, false );
  globalFieldList.addField( QStringLiteral("outward_postcode"), QVariant::String, true, false );
  globalFieldList.addField( QStringLiteral("valid_encrypted_postcode"), QVariant::Bool, false, false ); // Optional in most files, but required for database population
  globalFieldList.addField( QStringLiteral("encrypted_postcode"), QVariant::String, true, false );
  globalFieldList.addField( QStringLiteral("sample_id"), QVariant::String, true, false );
  globalFieldList.addField( QStringLiteral("sample_type"), QVariant::String, true, false );
  globalFieldList.addField( QStringLiteral("sample_date"), QVariant::Date, false, false );  // No longer required, as of BVDDB 1.4.3.
  globalFieldList.addField( QStringLiteral("sample_analysis_date"), QVariant::Date, false, false ); // No longer required, as of BVDDB 1.4.3.
  globalFieldList.addField( QStringLiteral("analysis_type"), QVariant::String, true, false );

  globalFieldList.addField( QStringLiteral("sample_result"), QVariant::String, true, false );
  globalFieldList.addFieldAlias( QStringLiteral("text_value"), QStringLiteral("sample_result") );// from APHA

  globalFieldList.addField( QStringLiteral("herd_result"), QVariant::String, false, false ); // Optional
  globalFieldList.addFieldAlias( QStringLiteral("bvd status"), QStringLiteral("herd_result") ); // from AHDB

  globalFieldList.addField( QStringLiteral("pooled_sample"), QVariant::Bool, false, false ); // Optional

  // Watch for the field name 'Result' in other data files: it could be trouble.
  globalFieldList.addField( QStringLiteral("bvd_history"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addFieldAlias( QStringLiteral("result"), QStringLiteral("bvd_history") );// from AHDB

  globalFieldList.addField( QStringLiteral("detailed_analysis_type"), QVariant::String, false, false ); // Optional.  Derived from APHA.
  globalFieldList.addFieldAlias( QStringLiteral("analysis"), QStringLiteral("detailed_analysis_type") );// from APHA

  globalFieldList.addField( QStringLiteral("parent_composite"), QVariant::String, false, false ); // Optional.  Supplied by BioBest for establishing whether samples are pooled.

  // Fields requiring encryption.  These are optional,
  // but their encrypted counterparts are generally required.
  //---------------------------------------------------------
  globalFieldList.addField( QStringLiteral("cph"), QVariant::String, false, true );
  globalFieldList.addFieldAlias( QStringLiteral("test_holding"), QStringLiteral("cph") );
  globalFieldList.addFieldAlias( QStringLiteral("farm's cph"), QStringLiteral("cph") );

  globalFieldList.addField( QStringLiteral("postcode"), QVariant::String, false, true );
  globalFieldList.addFieldAlias( QStringLiteral("postcodes"), QStringLiteral("postcode") );
  globalFieldList.addFieldAlias( QStringLiteral("responsible_keeper_postcode"), QStringLiteral("postcode") );
  globalFieldList.addFieldAlias( QStringLiteral("keeper_postcode"), QStringLiteral("postcode") );
  globalFieldList.addFieldAlias( QStringLiteral("farm's post code"), QStringLiteral("postcode") );

  globalFieldList.addField( QStringLiteral("animal_id"), QVariant::String, false, true );

  // More optional core database fields.
  //------------------------------------
  globalFieldList.addField( QStringLiteral("valid_encrypted_animal_id"), QVariant::Bool, false, false ); // Optional in most files, but required for database population
  globalFieldList.addField( QStringLiteral("encrypted_animal_id"), QVariant::String, false, false ); // Required only when pooled_sample. Encrypted.
  globalFieldList.addField( QStringLiteral("vp_reference"), QVariant::String, false, false );
  globalFieldList.addField( QStringLiteral("test_option_id"), QVariant::Int, false, false );
  globalFieldList.addField( QStringLiteral("checs_accredited_bvd_free"), QVariant::Bool, false, false );
  globalFieldList.addField( QStringLiteral("herd_size"), QVariant::Int, false, false );
  globalFieldList.addField( QStringLiteral("test_value"), QVariant::Double, false, false );
  globalFieldList.addField( QStringLiteral("positive_control_value"), QVariant::Double, false, false );
  globalFieldList.addField( QStringLiteral("negative_control_value"), QVariant::Double, false, false );
  globalFieldList.addField( QStringLiteral("herd_vaccination_status"), QVariant::String, false, false );

  globalFieldList.addField( QStringLiteral("herd_type"), QVariant::String, false, false );
  globalFieldList.addFieldAlias( QStringLiteral("column1"), QStringLiteral("herd_type") ); // from AHDB

  // Optional fields.  Written to text files if they appear, but not stored in the database.
  //----------------------------------------------------------------------------------------
  globalFieldList.addField( QStringLiteral("test done"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addField( QStringLiteral("action"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addField( QStringLiteral("pi hunt?"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addField( QStringLiteral("now pchs?"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addField( QStringLiteral("was vaccinating?"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addFieldAlias( QStringLiteral("was yaccinating?"), QStringLiteral("was vaccinating?") ); // from AHDB
  globalFieldList.addField( QStringLiteral("now vaccinating?"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addFieldAlias( QStringLiteral("now yaccinating?"), QStringLiteral("now vaccinating?") ); // from AHDB
  globalFieldList.addField( QStringLiteral("are you doing anything now?"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addField( QStringLiteral("after the meeting will you be doing anything differently?"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addField( QStringLiteral("has the event improved understanding?"), QVariant::String, false, false ); // from AHDB
  globalFieldList.addField( QStringLiteral("has the event given you practical ideas?"), QVariant::String, false, false ); // from AHDB

  globalFieldList.addField( QStringLiteral("submission_reason"), QVariant::String, false, false ); // from APHA

  globalFieldList.addField( QStringLiteral("registration date"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("farm type"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("farming system"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("herd type"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("breed"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("herd size"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("n animals"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("n adult breeding livestock"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("n breeding youngstock 0-12_months"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("n breeding youngstock 12_months+"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("n non breeding youngstock 0-12 months"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("n non breeding youngstock 12 months+"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("n purchased adult livestock"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("n purchased youngstock"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("housing"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("is organic"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("herd notes"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("is excluded from analysis"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("disease"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("disease value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("biosecurity strategy"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("biosecurity plan status"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("biosecurity plan value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("cattle"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("cattle value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("people"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("objects"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("disease entry risks"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("disease entry risks value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("disease spread risks"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("disease spread risks value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("disease status"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("disease status value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("vaccination status"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("vaccination status value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("control strategy"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("control plan status"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("control plan value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("surveillance strategy"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("surveillance plan status"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("surveillance plan value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("surveillance status"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("surveillance status value"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("date of awareness meeting (bvd 1)"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("date of 1:1 bvd 2"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("date of 1:1 bvd 3"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("date of 1:1 bvd 4"), QVariant::String, false, false ); // from Duchy
  globalFieldList.addField( QStringLiteral("interpretation made on fiq?"), QVariant::String, false, false ); // from Duchy

  // Any field names that do not appear above are ignored.
  //------------------------------------------------------
}


void clearGlobalFieldList() {
  globalFieldList.clear();
}


CBVDCoreFields::CBVDCoreFields() {
  buildLookups();
}


CBVDCoreFields::~CBVDCoreFields() {
  this->clear();
}


void CBVDCoreFields::addField(
    const QString& name,
    const QVariant::Type type,
    const bool isRequired,
    const bool requiresEncryption
) {
  if( _canonicalNames.contains( name ) ) {
    logMsg( QStringLiteral( "Duplication of canonical field name: %1" ).arg( name ) );
  }
  else {
    CBVDBaseField* cf = new CBVDBaseField( name, type, isRequired, requiresEncryption );
    _list.append( cf );
    _hash.insert( name, cf );
    _canonicalNames.insert( name );
  }
}


void CBVDCoreFields::addFieldAlias( const QString& lookupName, const QString& canonicalName ) {
  CBVDBaseField* cf;

  if( _hash.contains( lookupName ) || _canonicalNames.contains( lookupName ) ) {
    logMsg( QStringLiteral( "Duplication of field alias:: %1" ).arg( lookupName ) );
  }
  else if( _canonicalNames.contains( canonicalName ) ) {
    cf = _hash.value( canonicalName );
    _hash.insert( lookupName, cf );
  }
  else {
    cf = new CBVDBaseField( canonicalName, QVariant::String, false, false );

    // Only supported/recognized field names (those
    // listed in buildGlobalFieldList) go in the list.
    // All unique field names, however, are included in the hash.
    //_list.append( cf ); // See above.
    _hash.insert( lookupName, cf );
    _canonicalNames.insert( canonicalName );
  }
}


void CBVDCoreFields::clear() {
  // The mismatch here is intentional.
  // See comment in addFieldAlias for more.
  foreach( const QString& key, _canonicalNames.values() )
    delete _hash.value( key );

  _hash.clear();
  _canonicalNames.clear();
  _list.clear();
}


QVariant CBVDCoreFields::sampleType( const QVariant& key ) {
  if( key.isNull() )
    return QVariant( QVariant::String );
  else if( _sampleTypes.contains( key.toString().toLower() ) )
    return _sampleTypes.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

QVariant CBVDCoreFields::analysisType( const QVariant& key ) {
  if( key.isNull() )
    return QVariant( QVariant::String );
  else if( _analysisTypes.contains( key.toString().toLower() ) )
    return _analysisTypes.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

QVariant CBVDCoreFields::sampleResult( const QVariant& key ) {
  if( key.isNull() )
    return QVariant( QVariant::String );
  else if( _testResults.contains( key.toString().toLower() ) )
    return _testResults.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

QVariant CBVDCoreFields::vpReference( const QVariant& key ) {
  if( key.isNull() )
    return QVariant( QVariant::String );
  else if( _vpReferences.contains( key.toString().toLower() ) )
    return _vpReferences.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

QVariant CBVDCoreFields::herdStatus( const QVariant& key ) {
  if( key.isNull() )
    return QVariant( QVariant::String );
  else if( _herdStatuses.contains( key.toString().toLower() ) )
    return _herdStatuses.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

QVariant CBVDCoreFields::herdType( const QVariant& key ) {
  if( key.isNull() || ( "unknown" == key ) )
    return QVariant( QVariant::String );
  else if( _herdTypes.contains( key.toString().toLower() ) )
    return _herdTypes.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

QVariant CBVDCoreFields::vaccinationStatus( const QVariant& key ) {
  if( key.isNull() )
    return QVariant( QVariant::String );
  else if( _vaccinationStatuses.contains( key.toString().toLower() ) )
    return _vaccinationStatuses.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

QVariant CBVDCoreFields::testOption( const QVariant& key ) {
  if( key.isNull() )
    return QVariant( QVariant::String );
  else if( _testOptions.contains( key.toString().toLower() ) )
    return _testOptions.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

QVariant CBVDCoreFields::bvdHistory( const QVariant& key ) {
  if( key.isNull() )
    return QVariant( QVariant::String );
  else if( _bvdHistories.contains( key.toString().toLower() ) )
    return _bvdHistories.value( key.toString().toLower() );
  else
    return "INVALIDDATA";
}

bool CBVDCoreFields::isPooledIndicator( const QVariant& key ) {
  if( key.isNull() )
    return false;
  else if( multipleAnimalIds( key.toString() ) )
    return true;
  else
    return _pooledIndicators.contains( key.toString().toLower() );
}


void CBVDCoreFields::buildLookups() {
  // Recall that keys are always lower-case.

  _pooledIndicators.insert( QStringLiteral("bulk") );
  _pooledIndicators.insert( QStringLiteral("pooled") );
  _pooledIndicators.insert( QStringLiteral("bulk milk") );
  _pooledIndicators.insert( QStringLiteral("milk bulk") );

  _sampleTypes.insert( QStringLiteral("tissue"), QStringLiteral("Tissue") );
  _sampleTypes.insert( QStringLiteral("blood"), QStringLiteral("Blood") );
  _sampleTypes.insert( QStringLiteral("milk"), QStringLiteral("Milk") );

  // Additional sample types from BioBest
  _sampleTypes.insert( QStringLiteral("bulk_milk"), QStringLiteral("MilkBulk") );
  _sampleTypes.insert( QStringLiteral("colostrum"), QStringLiteral("Colustrum") );
  _sampleTypes.insert( QStringLiteral("colostrum"), QStringLiteral("Colostrum") );
  _sampleTypes.insert( QStringLiteral("csf"), QStringLiteral("CSF") );
  _sampleTypes.insert( QStringLiteral("e_membrane"), QStringLiteral("EggMembrane") );
  _sampleTypes.insert( QStringLiteral("ear_tissue"), QStringLiteral("EarTissue") );
  _sampleTypes.insert( QStringLiteral("faeces"), QStringLiteral("Faeces") );
  _sampleTypes.insert( QStringLiteral("feathers"), QStringLiteral("Feathers") );
  _sampleTypes.insert( QStringLiteral("filter_paper"), QStringLiteral("FilterPaper") );
  _sampleTypes.insert( QStringLiteral("fluid"), QStringLiteral("Fluid") );
  _sampleTypes.insert( QStringLiteral("iso_stix"), QStringLiteral("IsoStix") );
  _sampleTypes.insert( QStringLiteral("liv_kid"), QStringLiteral("LiverAndKidney") );
  _sampleTypes.insert( QStringLiteral("meat_processed"), QStringLiteral("MeatProcessed") );
  _sampleTypes.insert( QStringLiteral("meat_raw"), QStringLiteral("MeatRaw") );
  _sampleTypes.insert( QStringLiteral("milt"), QStringLiteral("Milt") );
  _sampleTypes.insert( QStringLiteral("mlt_kidney"), QStringLiteral("MiltAndKidney") );
  _sampleTypes.insert( QStringLiteral("of_kidney"), QStringLiteral("OvFluidAndKidney") );
  _sampleTypes.insert( QStringLiteral("other"), QStringLiteral("Other") );
  _sampleTypes.insert( QStringLiteral("ovarian"), QStringLiteral("Ovarian") );
  _sampleTypes.insert( QStringLiteral("pfizer_milk"), QStringLiteral("PfizerMilk") );
  _sampleTypes.insert( QStringLiteral("plasma"), QStringLiteral("Plasma") );
  _sampleTypes.insert( QStringLiteral("semen"), QStringLiteral("Semen") );
  _sampleTypes.insert( QStringLiteral("serum_swab"), QStringLiteral("SerumSwab") );
  _sampleTypes.insert( QStringLiteral("swab"), QStringLiteral("Swab") );
  _sampleTypes.insert( QStringLiteral("urine"), QStringLiteral("Urine") );
  //--
  _sampleTypes.insert( QStringLiteral("milkbulk"), QStringLiteral("MilkBulk") );
  _sampleTypes.insert( QStringLiteral("colustrum"), QStringLiteral("Colustrum") );
  _sampleTypes.insert( QStringLiteral("colostrum"), QStringLiteral("Colostrum") );
  _sampleTypes.insert( QStringLiteral("csf"), QStringLiteral("CSF") );
  _sampleTypes.insert( QStringLiteral("eggmembrane"), QStringLiteral("EggMembrane") );
  _sampleTypes.insert( QStringLiteral("eartissue"), QStringLiteral("EarTissue") );
  _sampleTypes.insert( QStringLiteral("faeces"), QStringLiteral("Faeces") );
  _sampleTypes.insert( QStringLiteral("feathers"), QStringLiteral("Feathers") );
  _sampleTypes.insert( QStringLiteral("filterpaper"), QStringLiteral("FilterPaper") );
  _sampleTypes.insert( QStringLiteral("fluid"), QStringLiteral("Fluid") );
  _sampleTypes.insert( QStringLiteral("isostix"), QStringLiteral("IsoStix") );
  _sampleTypes.insert( QStringLiteral("liverandkidney"), QStringLiteral("LiverAndKidney") );
  _sampleTypes.insert( QStringLiteral("meatprocessed"), QStringLiteral("MeatProcessed") );
  _sampleTypes.insert( QStringLiteral("meatraw"), QStringLiteral("MeatRaw") );
  _sampleTypes.insert( QStringLiteral("milt"), QStringLiteral("Milt") );
  _sampleTypes.insert( QStringLiteral("miltandkidney"), QStringLiteral("MiltAndKidney") );
  _sampleTypes.insert( QStringLiteral("ovfluidandkidney"), QStringLiteral("OvFluidAndKidney") );
  _sampleTypes.insert( QStringLiteral("other"), QStringLiteral("Other") );
  _sampleTypes.insert( QStringLiteral("ovarian"), QStringLiteral("Ovarian") );
  _sampleTypes.insert( QStringLiteral("pfizermilk"), QStringLiteral("PfizerMilk") );
  _sampleTypes.insert( QStringLiteral("plasma"), QStringLiteral("Plasma") );
  _sampleTypes.insert( QStringLiteral("semen"), QStringLiteral("Semen") );
  _sampleTypes.insert( QStringLiteral("serumswab"), QStringLiteral("SerumSwab") );
  _sampleTypes.insert( QStringLiteral("swab"), QStringLiteral("Swab") );
  _sampleTypes.insert( QStringLiteral("urine"), QStringLiteral("Urine") );


  // Additional sample types from APHA
  _sampleTypes.insert( QStringLiteral("abdominal_fluid"), QStringLiteral("AbdominalFluid") );
  _sampleTypes.insert( QStringLiteral("blclot"), QStringLiteral("BloodClotted") );
  _sampleTypes.insert( QStringLiteral("bledta"), QStringLiteral("BloodEDTA") );
  _sampleTypes.insert( QStringLiteral("bledta_pooled"), QStringLiteral("BloodEDTAPooled") );
  _sampleTypes.insert( QStringLiteral("blhep"), QStringLiteral("BloodHep") );
  _sampleTypes.insert( QStringLiteral("blhep_pooled"), QStringLiteral("BloodHepPooled") );
  _sampleTypes.insert( QStringLiteral("blood_pooled"), QStringLiteral("BloodPooled") );
  _sampleTypes.insert( QStringLiteral("bloxf"), QStringLiteral("BloodOxFl") );
  _sampleTypes.insert( QStringLiteral("bluns"), QStringLiteral("BloodUns") );
  _sampleTypes.insert( QStringLiteral("body_cavity_fluid"), QStringLiteral("BodyCavityFluid") );
  _sampleTypes.insert( QStringLiteral("brain"), QStringLiteral("Brain") );
  _sampleTypes.insert( QStringLiteral("brain_unspecified"), QStringLiteral("BrainUnspecified") );
  _sampleTypes.insert( QStringLiteral("ear_notch"), QStringLiteral("EarNotch") );
  _sampleTypes.insert( QStringLiteral("ear_tag"), QStringLiteral("EarNotch") );
  _sampleTypes.insert( QStringLiteral("foetal_fluid"), QStringLiteral("FoetalFluid") );
  _sampleTypes.insert( QStringLiteral("foetal_tiss_unspec"), QStringLiteral("FoetalTissueUnspec") );
  _sampleTypes.insert( QStringLiteral("foetal_tissue"), QStringLiteral("FoetalTissue") );
  _sampleTypes.insert( QStringLiteral("foetus_age_unk"), QStringLiteral("FoetusAgeUnknown") );
  _sampleTypes.insert( QStringLiteral("heart"), QStringLiteral("Heart") );
  _sampleTypes.insert( QStringLiteral("heart-blood"), QStringLiteral("HeartBlood") );
  _sampleTypes.insert( QStringLiteral("intestine_unspec"), QStringLiteral("IntestineUnspec") );
  _sampleTypes.insert( QStringLiteral("kidney"), QStringLiteral("Kidney") );
  _sampleTypes.insert( QStringLiteral("liver"), QStringLiteral("Liver") );
  _sampleTypes.insert( QStringLiteral("lung"), QStringLiteral("Lung") );
  _sampleTypes.insert( QStringLiteral("lymph_bm"), QStringLiteral("LymphMm") );
  _sampleTypes.insert( QStringLiteral("lymph_hep"), QStringLiteral("LymphHep") );
  _sampleTypes.insert( QStringLiteral("lymph_mesenteric"), QStringLiteral("LymphMesenteric") );
  _sampleTypes.insert( QStringLiteral("lymph_other"), QStringLiteral("LymphOther") );
  _sampleTypes.insert( QStringLiteral("milk_bulk"), QStringLiteral("MilkBulk") );
  _sampleTypes.insert( QStringLiteral("milk_pooled"), QStringLiteral("MilkPooled") );
  _sampleTypes.insert( QStringLiteral("mixed_sample_type"), QStringLiteral("MixedSampleType") );
  _sampleTypes.insert( QStringLiteral("mixed_viscera"), QStringLiteral("MixedViscera") );
  _sampleTypes.insert( QStringLiteral("pericardial_fluid"), QStringLiteral("PericardialFluid") );
  _sampleTypes.insert( QStringLiteral("peritoneal_fluid"), QStringLiteral("PeritonealFluid") );
  _sampleTypes.insert( QStringLiteral("placenta"), QStringLiteral("Placenta") );
  _sampleTypes.insert( QStringLiteral("plasma"), QStringLiteral("Plasma") );
  _sampleTypes.insert( QStringLiteral("pleural_fluid"), QStringLiteral("PleuralFluid") );
  _sampleTypes.insert( QStringLiteral("pooled_foet_fluid"), QStringLiteral("PooledFoetFluid") );
  _sampleTypes.insert( QStringLiteral("pooled_mes_lym_ileum"), QStringLiteral("PooledMesLymIleum") );
  _sampleTypes.insert( QStringLiteral("pooled_sample_mixed"), QStringLiteral("PooledSampleMixed") );
  _sampleTypes.insert( QStringLiteral("qa_sample"), QStringLiteral("QaSample") );
  _sampleTypes.insert( QStringLiteral("retropharyngeal_ln"), QStringLiteral("RetropharyngealLn") );
  _sampleTypes.insert( QStringLiteral("rna"), QStringLiteral("RNA") );
  _sampleTypes.insert( QStringLiteral("serum"), QStringLiteral("Serum") );
  _sampleTypes.insert( QStringLiteral("serum_pooled"), QStringLiteral("SerumPooled") );
  _sampleTypes.insert( QStringLiteral("sheath_washings"), QStringLiteral("SheathWashings") );
  _sampleTypes.insert( QStringLiteral("spleen"), QStringLiteral("Spleen") );
  _sampleTypes.insert( QStringLiteral("stomach"), QStringLiteral("Stomach") );
  _sampleTypes.insert( QStringLiteral("thoracic_fluid"), QStringLiteral("ThoracicFluid") );
  _sampleTypes.insert( QStringLiteral("thymus"), QStringLiteral("Thymus") );
  _sampleTypes.insert( QStringLiteral("thyroid"), QStringLiteral("Thyroid") );
  _sampleTypes.insert( QStringLiteral("tissue_culture_fluid"), QStringLiteral("TissueCultureFluid") );
  _sampleTypes.insert( QStringLiteral("tissue_unspecified"), QStringLiteral("TissueUnspecified") );
  _sampleTypes.insert( QStringLiteral("tissues_mixed"), QStringLiteral("TissuesMixed") );
  _sampleTypes.insert( QStringLiteral("unlisted"), QStringLiteral("Unlisted") );
  //--
  _sampleTypes.insert( QStringLiteral("abdominalfluid"), QStringLiteral("AbdominalFluid") );
  _sampleTypes.insert( QStringLiteral("bloodclotted"), QStringLiteral("BloodClotted") );
  _sampleTypes.insert( QStringLiteral("bloodedta"), QStringLiteral("BloodEDTA") );
  _sampleTypes.insert( QStringLiteral("bloodedtapooled"), QStringLiteral("BloodEDTAPooled") );
  _sampleTypes.insert( QStringLiteral("bloodhep"), QStringLiteral("BloodHep") );
  _sampleTypes.insert( QStringLiteral("bloodheppooled"), QStringLiteral("BloodHepPooled") );
  _sampleTypes.insert( QStringLiteral("bloodpooled"), QStringLiteral("BloodPooled") );
  _sampleTypes.insert( QStringLiteral("bloodoxfl"), QStringLiteral("BloodOxFl") );
  _sampleTypes.insert( QStringLiteral("blooduns"), QStringLiteral("BloodUns") );
  _sampleTypes.insert( QStringLiteral("bodycavityfluid"), QStringLiteral("BodyCavityFluid") );
  _sampleTypes.insert( QStringLiteral("brain"), QStringLiteral("Brain") );
  _sampleTypes.insert( QStringLiteral("brainunspecified"), QStringLiteral("BrainUnspecified") );
  _sampleTypes.insert( QStringLiteral("earnotch"), QStringLiteral("EarNotch") );
  _sampleTypes.insert( QStringLiteral("foetalfluid"), QStringLiteral("FoetalFluid") );
  _sampleTypes.insert( QStringLiteral("foetaltissueunspec"), QStringLiteral("FoetalTissueUnspec") );
  _sampleTypes.insert( QStringLiteral("foetaltissue"), QStringLiteral("FoetalTissue") );
  _sampleTypes.insert( QStringLiteral("foetusageunknown"), QStringLiteral("FoetusAgeUnknown") );
  _sampleTypes.insert( QStringLiteral("heart"), QStringLiteral("Heart") );
  _sampleTypes.insert( QStringLiteral("heartblood"), QStringLiteral("HeartBlood") );
  _sampleTypes.insert( QStringLiteral("intestineunspec"), QStringLiteral("IntestineUnspec") );
  _sampleTypes.insert( QStringLiteral("kidney"), QStringLiteral("Kidney") );
  _sampleTypes.insert( QStringLiteral("liver"), QStringLiteral("Liver") );
  _sampleTypes.insert( QStringLiteral("lung"), QStringLiteral("Lung") );
  _sampleTypes.insert( QStringLiteral("lymphmm"), QStringLiteral("LymphMm") );
  _sampleTypes.insert( QStringLiteral("lymphhep"), QStringLiteral("LymphHep") );
  _sampleTypes.insert( QStringLiteral("lymphmesenteric"), QStringLiteral("LymphMesenteric") );
  _sampleTypes.insert( QStringLiteral("lymphother"), QStringLiteral("LymphOther") );
  _sampleTypes.insert( QStringLiteral("milkbulk"), QStringLiteral("MilkBulk") );
  _sampleTypes.insert( QStringLiteral("milkpooled"), QStringLiteral("MilkPooled") );
  _sampleTypes.insert( QStringLiteral("mixedsampletype"), QStringLiteral("MixedSampleType") );
  _sampleTypes.insert( QStringLiteral("mixedviscera"), QStringLiteral("MixedViscera") );
  _sampleTypes.insert( QStringLiteral("pericardialfluid"), QStringLiteral("PericardialFluid") );
  _sampleTypes.insert( QStringLiteral("peritonealfluid"), QStringLiteral("PeritonealFluid") );
  _sampleTypes.insert( QStringLiteral("placenta"), QStringLiteral("Placenta") );
  _sampleTypes.insert( QStringLiteral("plasma"), QStringLiteral("Plasma") );
  _sampleTypes.insert( QStringLiteral("pleuralfluid"), QStringLiteral("PleuralFluid") );
  _sampleTypes.insert( QStringLiteral("pooledfoetfluid"), QStringLiteral("PooledFoetFluid") );
  _sampleTypes.insert( QStringLiteral("pooledmeslymileum"), QStringLiteral("PooledMesLymIleum") );
  _sampleTypes.insert( QStringLiteral("pooledsamplemixed"), QStringLiteral("PooledSampleMixed") );
  _sampleTypes.insert( QStringLiteral("qasample"), QStringLiteral("QaSample") );
  _sampleTypes.insert( QStringLiteral("retropharyngealln"), QStringLiteral("RetropharyngealLn") );
  _sampleTypes.insert( QStringLiteral("rna"), QStringLiteral("RNA") );
  _sampleTypes.insert( QStringLiteral("serum"), QStringLiteral("Serum") );
  _sampleTypes.insert( QStringLiteral("serumpooled"), QStringLiteral("SerumPooled") );
  _sampleTypes.insert( QStringLiteral("sheathwashings"), QStringLiteral("SheathWashings") );
  _sampleTypes.insert( QStringLiteral("spleen"), QStringLiteral("Spleen") );
  _sampleTypes.insert( QStringLiteral("stomach"), QStringLiteral("Stomach") );
  _sampleTypes.insert( QStringLiteral("thoracicfluid"), QStringLiteral("ThoracicFluid") );
  _sampleTypes.insert( QStringLiteral("thymus"), QStringLiteral("Thymus") );
  _sampleTypes.insert( QStringLiteral("thyroid"), QStringLiteral("Thyroid") );
  _sampleTypes.insert( QStringLiteral("tissueculturefluid"), QStringLiteral("TissueCultureFluid") );
  _sampleTypes.insert( QStringLiteral("tissueunspecified"), QStringLiteral("TissueUnspecified") );
  _sampleTypes.insert( QStringLiteral("tissuesmixed"), QStringLiteral("TissuesMixed") );
  _sampleTypes.insert( QStringLiteral("unlisted"), QStringLiteral("Unlisted") );

  //-----------------------------------------------------------------------------
  // DON'T FORGET: When adding new analysis types, update the function isPcrTest.
  //-----------------------------------------------------------------------------
  _analysisTypes.insert( QStringLiteral("bvdab"), QStringLiteral("Antibody") );
  _analysisTypes.insert( QStringLiteral("antibody"), QStringLiteral("Antibody") );
  _analysisTypes.insert( QStringLiteral("bvdag"), QStringLiteral("Antigen") );
  _analysisTypes.insert( QStringLiteral("antigen"), QStringLiteral("Antigen") );
  _analysisTypes.insert( QStringLiteral("pcr"), QStringLiteral("PCR") );

  // Additional analysis types from APHA
  _analysisTypes.insert( QStringLiteral("tc0123"), QStringLiteral("TC0123") );
  _analysisTypes.insert( QStringLiteral("tc0633"), QStringLiteral("TC0633") );
  _analysisTypes.insert( QStringLiteral("tc0655"), QStringLiteral("TC0655") );
  _analysisTypes.insert( QStringLiteral("tc0709"), QStringLiteral("TC0709") );
  _analysisTypes.insert( QStringLiteral("tc0758"), QStringLiteral("TC0758") );
  _analysisTypes.insert( QStringLiteral("tc0772"), QStringLiteral("TC0772") );
  _analysisTypes.insert( QStringLiteral("tc0390_2"), QStringLiteral("TC0390_2") );
  _analysisTypes.insert( QStringLiteral("tc0274"), QStringLiteral("TC0274") );
  _analysisTypes.insert( QStringLiteral("tc0574"), QStringLiteral("TC0574") );

  // Additional analysis types from BioBest
  _analysisTypes.insert( QStringLiteral("bvds"), QStringLiteral("BVDS") );
  _analysisTypes.insert( QStringLiteral("bvdv"), QStringLiteral("BVDV") );
  _analysisTypes.insert( QStringLiteral("bvd_pcr"), QStringLiteral("BVD_PCR") );
  _analysisTypes.insert( QStringLiteral("bvd_vnt"), QStringLiteral("BVD_VNT") );
  _analysisTypes.insert( QStringLiteral("bvd_vi"), QStringLiteral("BVD_VI") );

  // Additional analysis types from PCHS
  _analysisTypes.insert( QStringLiteral("bvdpcr"), QStringLiteral("BVDPCR") );

  //----------------------------------------------------------------------------------
  // DON'T FORGET: When adding new test result types, update the function worseResult.
  //----------------------------------------------------------------------------------
  _testResults.insert( QStringLiteral("unknown"), QStringLiteral("Unknown") );
  _testResults.insert( QStringLiteral("positive"), QStringLiteral("Positive") );
  _testResults.insert( QStringLiteral("high positive"), QStringLiteral("Positive") );
  _testResults.insert( QStringLiteral("low positive"), QStringLiteral("Positive") );
  _testResults.insert( QStringLiteral("not negative"), QStringLiteral("NotNegative") );
  _testResults.insert( QStringLiteral("not-negative"), QStringLiteral("NotNegative") );
  _testResults.insert( QStringLiteral("notnegative"), QStringLiteral("NotNegative") );
  _testResults.insert( QStringLiteral("negative"), QStringLiteral("Negative") );
  _testResults.insert( QStringLiteral("suspicious"), QStringLiteral("Suspect") );
  _testResults.insert( QStringLiteral("suspect"), QStringLiteral("Suspect") );
  _testResults.insert( QStringLiteral("no sample"), QStringLiteral("NoSample") );
  _testResults.insert( QStringLiteral("nosample"), QStringLiteral("NoSample") );
  _testResults.insert( QStringLiteral("i/s"), QStringLiteral("NoSample") ); // For now, treat "insufficient sample" as "no sample"
  _testResults.insert( QStringLiteral("inconclusive"), QStringLiteral("Inconclusive") );
  _testResults.insert( QStringLiteral("resample"), QStringLiteral("Inconclusive") );  // For now, treat "resample" as "inconclusive"
  _testResults.insert( QStringLiteral("re-sample"), QStringLiteral("Inconclusive") ); // Ditto.
  _testResults.insert( QStringLiteral("invalid"), QStringLiteral("Invalid") );
  _testResults.insert( QStringLiteral("n/a"), QStringLiteral("NA") );
  _testResults.insert( QStringLiteral("na"), QStringLiteral("NA") );

  _vpReferences.insert( QStringLiteral("farmer"), QStringLiteral("Farmer") );
  _vpReferences.insert( QStringLiteral("milk"), QStringLiteral("Milk") );
  _vpReferences.insert( QStringLiteral("vet"), QStringLiteral("Vet") );

  _herdStatuses.insert( QStringLiteral("negative"), QStringLiteral("Negative") );
  _herdStatuses.insert( QStringLiteral("n"), QStringLiteral("Negative") );
  _herdStatuses.insert( QStringLiteral("not negative"), QStringLiteral("NotNegative") );
  _herdStatuses.insert( QStringLiteral("not-negative"), QStringLiteral("NotNegative") );
  _herdStatuses.insert( QStringLiteral("notnegative"), QStringLiteral("NotNegative") );
  _herdStatuses.insert( QStringLiteral("y"), QStringLiteral("NotNegative") );
  _herdStatuses.insert( QStringLiteral("unknown"), QStringLiteral("Unknown") );
  _herdStatuses.insert( QStringLiteral("pending"), QStringLiteral("Pending") );
  _herdStatuses.insert( QStringLiteral("overdue"), QStringLiteral("Overdue") );
  _herdStatuses.insert( QStringLiteral("re"), QStringLiteral("Retest") );
  _herdStatuses.insert( QStringLiteral("not applicable"), QStringLiteral("NA") );
  _herdStatuses.insert( QStringLiteral("na"), QStringLiteral("NA") );

  _herdTypes.insert( QStringLiteral("beef"), QStringLiteral("Beef") );
  _herdTypes.insert( QStringLiteral("dairy"), QStringLiteral("Dairy") );
  _herdTypes.insert( QStringLiteral("beefanddairy"), QStringLiteral("BeefAndDairy") );

  // Herd types from APHA
  _herdTypes.insert( QStringLiteral("milk"), QStringLiteral("Dairy") );
  _herdTypes.insert( QStringLiteral("breeding"), QStringLiteral("Breeding") );
  _herdTypes.insert( QStringLiteral("captzoo"), QStringLiteral("CaptZoo") );
  _herdTypes.insert( QStringLiteral("farmed"), QStringLiteral("Farmed") );
  _herdTypes.insert( QStringLiteral("fibre"), QStringLiteral("Fibre") );
  _herdTypes.insert( QStringLiteral("finisher"), QStringLiteral("Finisher") );
  _herdTypes.insert( QStringLiteral("finishing"), QStringLiteral("Finisher") );
  _herdTypes.insert( QStringLiteral("hillsheep"), QStringLiteral("Hillsheep") );
  _herdTypes.insert( QStringLiteral("lowland"), QStringLiteral("Lowland") );
  _herdTypes.insert( QStringLiteral("na"), QStringLiteral("NA") );
  _herdTypes.insert( QStringLiteral("none"), QStringLiteral("None") );
  _herdTypes.insert( QStringLiteral("other"), QStringLiteral("Other") );
  _herdTypes.insert( QStringLiteral("otherfarm"), QStringLiteral("OtherFarm") );
  _herdTypes.insert( QStringLiteral("pet"), QStringLiteral("Pet") );
  _herdTypes.insert( QStringLiteral("rearing"), QStringLiteral("Rearing") );
  _herdTypes.insert( QStringLiteral("suckler"), QStringLiteral("Suckler") );
  _herdTypes.insert( QStringLiteral("wild"), QStringLiteral("Wild") );
  _herdTypes.insert( QStringLiteral("unknown"), QStringLiteral("Unknown") ); // Equivalent to null.  Sorted out in function herdType().

  _vaccinationStatuses.insert( QStringLiteral("yes"), QStringLiteral("Vaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("vaccinated"), QStringLiteral("Vaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("true"), QStringLiteral("Vaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("1"), QStringLiteral("Vaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("t"), QStringLiteral("Vaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("y"), QStringLiteral("Vaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("no"), QStringLiteral("Unvaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("unvaccinated"), QStringLiteral("Unvaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("not vaccinated"), QStringLiteral("Unvaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("false"), QStringLiteral("Unvaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("0"), QStringLiteral("Unvaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("f"), QStringLiteral("Unvaccinated") );
  _vaccinationStatuses.insert( QStringLiteral("n"), QStringLiteral("Unvaccinated") );

  _testOptions.insert( QStringLiteral("1"), QStringLiteral("1") );
  _testOptions.insert( QStringLiteral("2"), QStringLiteral("2") );
  _testOptions.insert( QStringLiteral("3"), QStringLiteral("3") );
  _testOptions.insert( QStringLiteral("4"), QStringLiteral("4") );
  _testOptions.insert( QStringLiteral("5"), QStringLiteral("5") );
  _testOptions.insert( QStringLiteral("6"), QStringLiteral("6") );
  _testOptions.insert( QStringLiteral("7"), QStringLiteral("7") );

  _bvdHistories.insert( QStringLiteral("a"), QStringLiteral("Amber") );
  _bvdHistories.insert( QStringLiteral("r"), QStringLiteral("Red") );
  _bvdHistories.insert( QStringLiteral("g"), QStringLiteral("Green") );
}


QString CBVDCoreFields::findCanonicalName( const QString& lookupName ) {
  if( _hash.contains( lookupName ) )
    return _hash.value( lookupName )->name();
  else
    return QLatin1String("");
}


bool CBVDCoreFields::fieldHeadersOK( const QStringList& fieldsInFile, QStringList& missingFields ) {

  // Figure out what the canonical names are for the names provided in the file.
  QSet<QString> cfif; // canonicalFieldsInFile
  QString tmp;
  for( int i = 0; i < fieldsInFile.count(); ++i ) {
    tmp = globalFieldList.findCanonicalName( fieldsInFile.at(i) );
    if( !tmp.isEmpty() )
      cfif.insert( tmp );
  }

  // Make a list of all of the required fields that DON'T show up in the field list for this file.
  for( int i = 0; i < globalFieldList.fieldCount(); ++i ) {
    if( globalFieldList.at(i)->isRequired() ) {
      QString fn = globalFieldList.at(i)->name();
      if( !cfif.contains( fn ) )
        missingFields.append( fn );
    }
  }

  // CPH and postcode are funny: raw input files will not have the required fields
  // encrypted_cph, encrypted_postcode, etc., as they are generated from data as needed.
  // Encrypted files, on the other hand, will have all of these fields.
  // This function needs to be smart enough to sort that out.
  if( cfif.contains( QStringLiteral("cph") ) ) {
    missingFields.removeAll( QStringLiteral("encrypted_cph") );
    missingFields.removeAll( QStringLiteral("county") );
    missingFields.removeAll( QStringLiteral("parish") );
  }

  if( cfif.contains( QStringLiteral("postcode") ) ) {
    missingFields.removeAll( QStringLiteral("outward_postcode") );
    missingFields.removeAll( QStringLiteral("encrypted_postcode") );
  }

  // If anything is left in the "missing" list, that's a problem.
  return ( 0 == missingFields.count() );
}


CBVDField CBVDCoreFields::buildField( const QString& inputName, const QString& canonicalName, QString data, const int lineID, CErrorList& errMsgs ) {
  CBVDBaseField* bf = globalFieldList.at( canonicalName );

  bf->setFieldIsPresent( true );

  data = data.trimmed();

  if( ("-" == data) || ("nil" == data.toLower()) || ("null" == data.toLower()) ) {
    data = QLatin1String("");
  }

  CBVDField cf( canonicalName, bf->expectedType(), bf->isRequired(), bf->requiresEncryption() );

  // If this field is required, make sure a value is given.
  //-------------------------------------------------------
  if( data.isEmpty() ) {
    cf.setData( QVariant( bf->expectedType() ) );

    if( bf->isRequired() ) {
      cf.setValid( false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: Required field %2 is missing." ).arg( lineID ).arg( inputName ) );
    }
  }

  // No validation required for lab_test_reference.
  //-----------------------------------------------
  else if( "lab_test_reference" == canonicalName )
    cf.setData( data, true );

  // No validation required for sample_id.
  //--------------------------------------
  else if( "sample_id" == canonicalName )
    cf.setData( data, true );

  // pooled_sample must be a valid boolean.
  //---------------------------------------
  else if( "pooled_sample" == canonicalName ) {
    bool pooledSample, ok;
    pooledSample = strToBool( data, &ok );
    if( !ok ) {
      pooledSample = false;
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( pooledSample, true );
    }
  }

  // sample_type must be a recognized value.
  //----------------------------------------
  else if( "sample_type" == canonicalName ) {
    if( !_sampleTypes.contains( data.toLower() ) ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not recognized." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( sampleType( data ), true );
    }
  }

  // sample_date must be a valid date.
  //----------------------------------
  else if( "sample_date" == canonicalName ) {
    QDate date = guessDateFromString( data, StrUtilsDateFormat::UKDateFormat, 2000 );
    if( !date.isValid() ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not a valid date." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( date, true );
    }
  }

  // sample_analysis_date must be a valid date.
  //-------------------------------------------
  else if( "sample_analysis_date" == canonicalName ) {
    QDate date = guessDateFromString( data, StrUtilsDateFormat::UKDateFormat, 2000 );
    if( !date.isValid() ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not a valid date." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( date, true );
    }
  }

  // analysis_type must be a recognized value.
  //------------------------------------------
  else if("analysis_type" == canonicalName ) {
    if( !_analysisTypes.contains( data.toLower() ) ) {
     cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not recognized." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
     cf.setData( analysisType( data ), true );
    }
  }

  // sample_result must be a recognized value.
  //------------------------------------------
  else if( "sample_result" == canonicalName ) {
    // Data from BioBest includes numeric test scores as well as sample results in the same field.
    // This has to be sorted out either during post-validation.
    if( reBioBestSampleResult.exactMatch( data ) )
      cf.setData( data, true );
    else if( _testResults.contains( data.toLower() ) )
      cf.setData( sampleResult( data ), true );
    else {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not recognized." ).arg( lineID ).arg( data, inputName ) );
    }  
  }

  // vp_reference must be a recognized value.
  //-----------------------------------------
  else if( "vp_reference" == canonicalName ) {
    if( CPostcode::isPostcode( data ) )
      cf.setData( vpReference( QVariant( "vet" ) ), true );
    else if( _vpReferences.contains( data.toLower() ) )
      cf.setData( vpReference( data ), true );
    else {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not recognized." ).arg( lineID ).arg( data, inputName ) );
    }
  }

  // herd_result must be a recognized value.
  //----------------------------------------
  else if( "herd_result" == canonicalName ) {
    if( !_herdStatuses.contains( data.toLower() ) ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not recognized." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( herdStatus( data ), true );
    }
  }

  // test_option_id must be an integer value 1 - 7 if provided.
  //-----------------------------------------------------------
  else if( "test_option_id" == canonicalName ) {
    bool ok;
    int temp;
    temp = data.toInt( &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else if( ( 1 > temp ) || ( 7 < temp ) ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not valid." ).arg( lineID ).arg( temp ).arg( inputName ) );
    }
    else {
      cf.setData( temp, true );
    }
  }

  // checs_accredited_bvd_free must be a bool if provided.
  //------------------------------------------------------
  else if( "checs_accredited_bvd_free" == canonicalName ) {
    bool temp, ok;
    temp = strToBool( data, &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( temp, true );
    }
  }

  // herd_size must be an integer if provided.
  //------------------------------------------
  else if( "herd_size" == canonicalName ) {
    bool ok;
    int temp;
    temp = data.toInt( &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( temp, true );
    }
  }

  // test_value must be numeric if provided.
  //----------------------------------------
  else if( "test_value" == canonicalName ) {
    bool ok;
    double temp;
    temp = data.toDouble( &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( temp, true );
    }
  }

  // positive_control_value must be numeric if provided.
  //----------------------------------------------------
  else if( "positive_control_value" == canonicalName ) {
    bool ok;
    double temp;
    temp = data.toDouble( &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( temp, true );
    }
  }

  // negative_control_value must be numeric if provided.
  //----------------------------------------------------
  else if( "negative_control_value" == canonicalName ) {
    bool ok;
    double temp;
    temp = data.toDouble( &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( temp, true );
    }
  }

  // herd_vaccination_status must be "yes" or "no" if provided.
  //-----------------------------------------------------------
  else if( "herd_vaccination_status" == canonicalName ) {
    if( !_vaccinationStatuses.contains( data.toLower() ) ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not recognized." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( vaccinationStatus( data ), true );
    }
  }

  // herd_type must be "beef" or "dairy" if provided.
  //-------------------------------------------------
  else if( "herd_type" == canonicalName ) {
    if( !_herdTypes.contains( data.toLower() ) ) {
      cf.setData( data, false );
      errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not recognized." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( herdType( data ), true );
    }
  }


  // bvd_history must be a recognized value.
  //----------------------------------------
  else if( "bvd_history" == canonicalName ) {
    if( !_bvdHistories.contains( data.toLower() ) ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not recognized." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( bvdHistory( data ), true );
    }
  }

  // These appear only in files that have already been encrypted.
  //-------------------------------------------------------------

  else if( "encrypted_animal_id" == canonicalName ) {
    // This will have to be double-checked in postValidate(), as validity depends on valid_encrypted_animal_id.
    cf.setData( data );
  }

  else if( "valid_encrypted_animal_id" == canonicalName ) {
    if( data.isEmpty() )
      cf.setData( QVariant(QVariant::Bool), true );
    else {
      bool temp, ok;
      temp = strToBool( data, &ok );
      if( !ok ) {
        cf.setData( data, false );
        errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
      }
      else {
        cf.setData( temp, true );
      }
    }
  }

  else if( "encrypted_cph" == canonicalName ) {
    // This will have to be double-checked in postValidate(), as validity depends on valid_encrypted_cph.
    cf.setData( data );
  }

  else if( "valid_encrypted_cph" == canonicalName ) {
    bool temp, ok;
    temp = strToBool( data, &ok );
    if( !ok ) {
      cf.setData( "data", false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( temp, true );
    }
  }

  else if( "county" == canonicalName ) {
    bool ok;
    int county = data.toInt( &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not valid." ).arg( lineID ).arg( data,inputName ) );
    }
    else {
      if( !CCPH::isGBCounty( county ) ) {
        cf.setData( county, false );
        errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not valid." ).arg( lineID ).arg( county ).arg( inputName ) );
      }
      else
        cf.setData( county, true );
    }
  }

  else if( "parish" == canonicalName ) {
    bool ok;
    int parish = data.toInt( &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not valid." ).arg( lineID ).arg( data, inputName ) );
    }
    else { // parish should be an integer value up to 999
      if( 1000 <= parish ) {
        cf.setData( parish, false );
        errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not valid." ).arg( lineID ).arg( data, inputName ) );
      }
      else
        cf.setData( parish, true );
    }
  }

  else if( "valid_encrypted_postcode" == canonicalName ) {
    bool temp, ok;
    temp = strToBool( data, &ok );
    if( !ok ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      cf.setData( temp, true );
    }
  }

  else if( "encrypted_postcode" == canonicalName ) {
    // This will have to be double-checked in postValidate(), as validity depends on valid_encrypted_postcode.
    cf.setData( data );
  }

  else if( "outward_postcode" == canonicalName ) {
    QString tmp = data;
    bool isPC = CPostcode::isOutwardPostcode( tmp );
    if( !isPC ) {
      cf.setData( data, false );
      errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not valid." ).arg( lineID ).arg( data, inputName ) );
    }
    else
      cf.setData( data, true );
  }

  // This catches any other recognized canonical name.
  // Fields that aren't recognized are ignored.
  //--------------------------------------------------
  else if( _canonicalNames.contains( canonicalName ) ) {
    cf.setData( data, true );
  }

  return cf;
}



CBVDRecord::CBVDRecord() {
  initialize();
}


CBVDRecord::CBVDRecord( QCsvObject* input, const int line, const QString& passphrase, const bool excludeParish ) {
  initialize();

  _excludeParish = excludeParish;

  processCsvFields( input, line, passphrase );
}


void CBVDRecord::initialize() {
  _errMsgs.clear();

  _excludeParish = false;

  for( int i = 0; i < globalFieldList.fieldCount(); ++i ) {
    CBVDBaseField* cf = globalFieldList.at(i);
    _fieldData.insert( cf->name(), new CBVDField( cf->name(), cf->expectedType(), cf->isRequired(), cf->requiresEncryption() ) );
  }
}


// NOTE:  This function performs some minimal validation of core fields.
// It does not confirm that all required core fields are present, nor
// does it check that field relationships/dependencies make sense.
//
// These more advanced checks, while required prior to storage of data
// in the BVD repository, cannot be done on the data provider side, as
// data formats differ too much.  Ensuring consistency of collected data
// will be critical in the future.
void CBVDRecord::buildEncryptedField( const QString& canonicalName, const QString& inputName, QString data, const int lineID, const QString& passphrase ) {
  if( ("-" == data) || ("nil" == data.toLower()) )
    data = QLatin1String("");

  // Handle unencrypted CPH number.
  //-------------------------------
  if( "cph" == canonicalName ) {
    globalFieldList.at( QStringLiteral("encrypted_cph") )->setFieldIsPresent( true );
    globalFieldList.at( QStringLiteral("county") )->setFieldIsPresent( true );
    globalFieldList.at( QStringLiteral("parish") )->setFieldIsPresent( true );
    globalFieldList.at( QStringLiteral("valid_encrypted_cph") )->setFieldIsPresent( true );

    if( data.isEmpty() ) {
      _fieldData.value( QStringLiteral("encrypted_cph") )->setData( QVariant( QVariant::String ), false );
      _fieldData.value( QStringLiteral("valid_encrypted_cph") )->setData( false, true );
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %2 is missing." ).arg( lineID ).arg( inputName ) );
    }
    else if( !CCPH::isCph( data ) ) {
      _fieldData.value( QStringLiteral("encrypted_cph") )->setData( sha( data, passphrase ), false );
      _fieldData.value( QStringLiteral("valid_encrypted_cph") )->setData( false, true );
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not properly formatted." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      CCPH cph( data );
      if( !cph.isValid() ) {
        _fieldData.value( QStringLiteral("encrypted_cph") )->setData( sha( data, passphrase ), false );
        _fieldData.value( QStringLiteral("valid_encrypted_cph") )->setData( false, true );
        _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not valid." ).arg( lineID ).arg( data, inputName ) );
      }
      else {
        _fieldData.value( QStringLiteral("encrypted_cph") )->setData( sha( cph.toString( CCPH::DefaultCPH ), passphrase ), true );
        _fieldData.value( QStringLiteral("county") )->setData( cph.county(), true );
        _fieldData.value( QStringLiteral("parish") )->setData( cph.parish(), true );
        _fieldData.value( QStringLiteral("valid_encrypted_cph") )->setData( true, true );
      }
    }
  }

  // Handle unencrypted postcode.
  //-----------------------------
  else if( "postcode" == canonicalName ) {
    globalFieldList.at( QStringLiteral("encrypted_postcode") )->setFieldIsPresent( true );
    globalFieldList.at( QStringLiteral("outward_postcode") )->setFieldIsPresent( true );
    globalFieldList.at( QStringLiteral("valid_encrypted_postcode") )->setFieldIsPresent( true );

    if( data.isEmpty() ) {
      _fieldData.value( QStringLiteral("encrypted_postcode") )->setData( QVariant( QVariant::String ), false );
      _fieldData.value( QStringLiteral("valid_encrypted_postcode") )->setData( false, true );
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %2 is missing." ).arg( lineID ).arg( inputName ) );
    }
    else if( !CPostcode::isPostcode( data ) ) {
      _fieldData.value( QStringLiteral("encrypted_postcode") )->setData( sha( data, passphrase ), false );
      _fieldData.value( QStringLiteral("valid_encrypted_postcode") )->setData( false, true );
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: %3 (%2) is not valid." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      _fieldData.value( QStringLiteral("encrypted_postcode") )->setData( sha( data, passphrase ), true );
      _fieldData.value( QStringLiteral("outward_postcode") )->setData( data.split( QRegExp( "\\s" ) ).at(0), true );
      _fieldData.value( QStringLiteral("valid_encrypted_postcode") )->setData( true, true );
    }
  }

  // Handle unencrypted animal_id.
  //------------------------------
  else if( "animal_id" == canonicalName ) {
    globalFieldList.at( QStringLiteral("encrypted_animal_id") )->setFieldIsPresent( true );
    globalFieldList.at( QStringLiteral("valid_encrypted_animal_id") )->setFieldIsPresent( true );

    if( globalFieldList.isPooledIndicator( data.toLower() ) ) {
      _fieldData.value( QStringLiteral("encrypted_animal_id") )->setData( QVariant(QVariant::String), true );
      _fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->setData( QVariant(QVariant::Bool), true );
    }
    else if( !isAnimalID( data ) ) {
      _fieldData.value( QStringLiteral("encrypted_animal_id") )->setData( shaAnimalID( data, passphrase ), false );
      _fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->setData( false, true );
      _errMsgs.append( CError::Warning, QStringLiteral( "Line %1: %3 (%2) is not a valid animal ID/eartag number." ).arg( lineID ).arg( data, inputName ) );
    }
    else {
      _fieldData.value( QStringLiteral("encrypted_animal_id") )->setData( shaAnimalID( data, passphrase ), true );
      _fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->setData( true, true );
    }
  }

  // Handle unencrypted lab test reference.
  // (Currently required for NML data.)
  //---------------------------------------
  else if( "lab_test_reference" == canonicalName ) {
    globalFieldList.at( QStringLiteral("lab_test_reference") )->setFieldIsPresent( true );
    _fieldData.value( QStringLiteral("lab_test_reference") )->setData( sha( data, passphrase ), true );
  }
}


void CBVDRecord::processCsvFields( QCsvObject* input, const int line, const QString& passphrase ) {
  int lineID = input->nCommentRows() + line + 1;

  QString inputName, canonicalName, data;
  for( int i = 0; i < input->fieldCount(); ++i ) {
    inputName = input->fieldNames().at(i);
    canonicalName = globalFieldList.findCanonicalName( inputName );
    data = input->field( i );

    if( !canonicalName.isEmpty() ) {
      if( globalFieldList.at( canonicalName )->requiresEncryption() )
        buildEncryptedField( canonicalName, inputName, data, lineID, passphrase );
      else {
        CBVDField cf = globalFieldList.buildField( inputName, canonicalName, data, lineID, _errMsgs );
        _fieldData.value( canonicalName )->setData( cf.data(), cf.isValid() );
      }
    }
  }

  postValidate( lineID );
}


bool CBVDRecord::isPcrTest( QString analysisType ) {
  analysisType = analysisType.trimmed().toLower();

  return(
      ( "pcr" == analysisType )
    ||
      ( "bvd_pcr" == analysisType )
    ||
      ( "bvdpcr" == analysisType )
    ||
      ( "tc0655" == analysisType )
    ||
      ( "tc0709" == analysisType )
    ||
      ( "tc0758" == analysisType )
  );
}


int toNumeric( QString result ) {
  result = result.toLower();

  if( "na" == result )
    return 0;
  else if( "unknown" == result )
    return 1;
  else if( "invalid" == result )
    return 1;
  else if( "nosample" == result )
    return 1;
  else if( "negative" == result )
    return 2;
  else if( "inconclusive" == result )
    return 3;
  else if( "suspect" == result )
    return 4;
  else if( "notnegative" == result )
    return 5;
  else if( "low positive" == result )
    return 6;
  else if( "positive" == result )
    return 7;
  else if( "high positive" == result )
    return 8;
  else
    return 0;
}


QString CBVDRecord::worseResult( QString testResult1, QString testResult2 ) {
  int res1 = toNumeric( testResult1 );
  int res2 = toNumeric( testResult2 );

  if( res1 > res2 )
    return testResult1;
  else
    return testResult2;
}


void CBVDRecord::postValidate( const int lineID ) {
  bool isSha, dataIsValid, wasValid;
  QString data;

  // Encrypted animal IDs are valid if the following two conditions are met:
  //   - The original animal ID was properly formatted.  This is recorded in valid_encrypted_animal_id.
  //   - The encrypted animal ID is properly SHA-encrypted.  (Unless a file has been tampered with, it will be.)
  //------------------------------------------------------------------------------------------------------------
  if( !_fieldData.value( QStringLiteral("encrypted_animal_id") )->isNull() ){
    data = _fieldData.value( QStringLiteral("encrypted_animal_id") )->data().toString();
    isSha = isShaEncrypted( data.right( data.length() - 2 ) );

    dataIsValid = (
      !_fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->isNull()
      && _fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->isValid()
      && _fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->data().toBool()
    );

    wasValid = _fieldData.value( QStringLiteral("encrypted_animal_id") )->isValid();
    _fieldData.value( QStringLiteral("encrypted_animal_id") )->setValid( isSha && dataIsValid );

    if( !isSha )
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: SHA encryption of animal ID is not valid." ).arg( lineID ) );

    if( wasValid && !dataIsValid )
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: Animal ID not valid (missing, unexpected, or bad format)." ).arg( lineID ) );
  }


  // Encrypted postcodes are valid if:
  //  1) The original postcode was properly formatted.  This is recorded in valid_encrypted_postcode.
  //  2) The encrypted postcode is properly SHA-encrypted.  (Unless a file has been tampered with, it will be.)
  //-----------------------------------------------------------------------------------------------------------
  if( !_fieldData.value( QStringLiteral("encrypted_postcode") )->isNull() ) {
    data = _fieldData.value( QStringLiteral("encrypted_postcode") )->data().toString();
    isSha = isShaEncrypted( data );

    dataIsValid = (
      !_fieldData.value( QStringLiteral("valid_encrypted_postcode") )->isNull()
      && _fieldData.value( QStringLiteral("valid_encrypted_postcode") )->isValid()
      && _fieldData.value( QStringLiteral("valid_encrypted_postcode") )->data().toBool()
    );

    wasValid = _fieldData.value( QStringLiteral("encrypted_postcode") )->isValid();
    _fieldData.value( QStringLiteral("encrypted_postcode") )->setValid( isSha && dataIsValid );

    if( !isSha )
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: SHA encryption of postcode is not valid." ).arg( lineID ) );

    if( wasValid && !dataIsValid )
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: Postcode is missing or not valid." ).arg( lineID ) );
  }


  // Encrypted CPHs are valid if:
  //  1) The original CPH was properly formatted.  This is recorded in valid_encrypted_cph.
  //  2) The encrypted CPH is properly SHA-encrypted.  (Unless a file has been tampered with, it will be.)
  //-------------------------------------------------------------------------------------------------------
  if( !_fieldData.value( QStringLiteral("encrypted_cph") )->isNull() ) {
    data = _fieldData.value( QStringLiteral("encrypted_cph") )->data().toString();
    isSha = isShaEncrypted( data );

    dataIsValid = (
      !_fieldData.value( QStringLiteral("valid_encrypted_cph") )->isNull()
      && _fieldData.value( QStringLiteral("valid_encrypted_cph") )->isValid()
      && _fieldData.value( QStringLiteral("valid_encrypted_cph") )->data().toBool()
    );

    wasValid = _fieldData.value( QStringLiteral("encrypted_cph") )->isValid();
    _fieldData.value( QStringLiteral("encrypted_cph") )->setValid( isSha && dataIsValid );

    if( !isSha )
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: SHA encryption of CPH is not valid." ).arg( lineID ) );

    if( wasValid && !dataIsValid )
      _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: CPH is missing or not valid." ).arg( lineID ) );
  }


  // Generally, pooled samples should not have animal IDs, and records with animal IDs should not be pooled samples.
  //----------------------------------------------------------------------------------------------------------------
  // If the value for pooled sample is not valid, that's a problem.
  if( _fieldData.value( QStringLiteral("pooled_sample") )->isNull() || !_fieldData.value( QStringLiteral("pooled_sample") )->isValid() ) {
    wasValid = _fieldData.value( QStringLiteral("pooled_sample") )->isValid();
    _fieldData.value( QStringLiteral("pooled_sample") )->setValid( false );

    if( wasValid )
      _errMsgs.append(CError::Warning, QStringLiteral( "Line %1: The value for pooled sample is not valid." ).arg( lineID ) );
  }

  // If the value for pooled sample is valid, let's see what else is going on.
  else {
    // If sample is not pooled, then there must be an animal ID.
    if( !_fieldData.value( QStringLiteral("pooled_sample") )->data().toBool() ) {
      // For the time being, the animal ID does not need to be valid, as long as one is present.
      if( _fieldData.value( QStringLiteral("encrypted_animal_id") )->isNull() /* || !animalIDField->isValid() */ ){
        _fieldData.value( QStringLiteral("pooled_sample") )->setValid( false );
        _errMsgs.append( CError::Warning, QStringLiteral( "Line %1: An animal ID is required for unpooled samples." ).arg( lineID ) );
      }
    }

    // If sample is pooled, then animal_id should be a null value, except in the case of PCR samples.
    // If it's not, there might be something funny.
    else if( !_fieldData.value( QStringLiteral("encrypted_animal_id") )->isNull() ) {

      // For PCR tests with animal IDs, an invalid animal ID is a problem.
      if( isPcrTest( _fieldData.value( QStringLiteral("analysis_type") )->data().toString() ) ) {
        if( !_fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->data().toBool() ) {
          _fieldData.value( QStringLiteral("pooled_sample") )->setValid( false );
          _errMsgs.append( CError::Warning, QStringLiteral( "Line %1: A pooled PCR test with an invalid animal ID has been found." ).arg( lineID ) );
        }
      }

      // Is this was not a PCR test but a valid animal ID is present, record an error.
      else if( _fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->data().toBool() ) {
        _fieldData.value( QStringLiteral("pooled_sample") )->setValid( false );
        _errMsgs.append( CError::Warning, QStringLiteral( "Line %1: An animal ID is not expected for pooled samples." ).arg( lineID ) );
      }

      // Otherwise, just blow it away.
      // There is no way to know what kind of garbage might have been recorded in this field.
      else {
        _fieldData.value( QStringLiteral("encrypted_animal_id") )->setData( QVariant( QVariant::String ) );
        _fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->setData( QVariant( QVariant::Bool ) );
        _fieldData.value( QStringLiteral("valid_encrypted_animal_id") )->setData( true );
      }

    }
  }  


  // County and parish may be valid separately without being a valid combination.
  // FIXME: For now, this is a very crude (read: entirely useless) check.
  // Some day, the function CCPH::isGBParish should be made to be more robust.
  //------------------------------------------------------------------------------
  CBVDField* countyField;
  CBVDField* parishField;

  if( _fieldData.contains( QStringLiteral("county") ) )
    countyField = _fieldData.value( QStringLiteral("county") );
  else
    countyField = nullptr;

  if( _fieldData.contains( QStringLiteral("parish") ) )
    parishField = _fieldData.value( QStringLiteral("parish") );
  else
    parishField = nullptr;

  if( !(nullptr == countyField) && !(nullptr == parishField) ) {
    if( countyField->isValid() && parishField->isValid() ) {
      if( !CCPH::isGBParish( parishField->data().toInt(), countyField->data().toInt() ) ) {
        _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: County/parish combination is not valid." ).arg( lineID ) );
      }
    }
  }

  // County and parish must exist and be valid for CPH to be valid.
  // (Except for PCHS data, where only county is provided.)
  //---------------------------------------------------------------
  if( _excludeParish ) {
    if(
      _fieldData.value( QStringLiteral("encrypted_cph") )->isValid()
      && !( _fieldData.contains( QStringLiteral("county") ) && _fieldData.value( QStringLiteral("county") )->isValid() )
    ) {
      if( _fieldData.contains( QStringLiteral("encrypted_cph") ) ) {
        _fieldData.value( QStringLiteral("encrypted_cph") )->setValid( false );
        _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: CPH cannot be valid without valid county." ).arg( lineID ) );
      }
    }
  }
  else {
    if(
      _fieldData.value( QStringLiteral("encrypted_cph") )->isValid()
      && !( _fieldData.contains( QStringLiteral("county") ) && _fieldData.value( QStringLiteral("county") )->isValid() && _fieldData.contains( QStringLiteral("parish") ) && _fieldData.value( QStringLiteral("parish") )->isValid() )
    ) {
      if( _fieldData.contains( QStringLiteral("encrypted_cph") ) ) {
        _fieldData.value( QStringLiteral("encrypted_cph") )->setValid( false );
        _errMsgs.append( CError::Critical, QStringLiteral( "Line %1: CPH cannot be valid without valid county and parish." ).arg( lineID ) );
      }
    }
  }

  // Data from BioBest may have numeric test values mixed in with sample results for antibody tests.
  //------------------------------------------------------------------------------------------------
  if( _fieldData.value( QStringLiteral("sample_result") )->isValid() && reBioBestSampleResult.exactMatch( _fieldData.value( QStringLiteral("sample_result") )->data().toString() )  )
    postProcessBioBestValue( lineID );
}


// Data from BioBest may have numeric test values mixed in with sample results for antibody tests.
// This block resolves such situations, based on the following information provided by BioBest:
//   For blood (serum) samples: <9 is Negative.  (9+ assumed to be NotNegative)
//   For individual milk samples: <9 is Negative.  (9+ assumed to be NotNegative)
//   For bulk milk samples:
//     > 100: Most cows immune and herd may contain PIs - a check test is indicated (NotNegative)
//     21 - 100: Proportion of herd immune/vaccinated – infection may be currently active (NotNegative)
//     3 - 20: Some immune/vaccinated animals – active infection unlikely. (Treat as Inconclusive)
//     <3: Herd naïve/unvaccinated (Negative)
// Note that, currently, BioBest is the only data provider to do this.  If others start to do the
// same thing, then a flag will have to be set in each file to establish which provider sent it.
//------------------------------------------------------------------------------------------------
void CBVDRecord::postProcessBioBestValue(const int lineID ) {
  // Extract the test value from the sample result field.
  //-----------------------------------------------------
  // We already know that this matches the BioBest format.
  QString valText = _fieldData.value( QStringLiteral("sample_result") )->data().toString();

  double testVal;
  bool ok;
  if( valText.contains( '<' ) ) {
    valText.replace( '<', QLatin1String("") );
    testVal = valText.trimmed().toDouble( &ok );
    if( ok )
      testVal = testVal - 0.1;
  }
  else if( valText.contains( '>' ) ) {
    valText.replace( '>', QLatin1String("") );
    testVal = valText.trimmed().toDouble( &ok );
    if( ok )
      testVal = testVal + 0.9;
  }
  else
    testVal = valText.trimmed().toDouble( &ok );

  if(!ok) {
    _fieldData.value( QStringLiteral("sample_result") )->setData( QVariant(), false );
    _fieldData.value( QStringLiteral("test_value") )->setData( QVariant(), false );
    _errMsgs.append( CError::Warning, QStringLiteral( "Line %1: Invalid sample result." ).arg( lineID ) );
    return;
  }

  // If we get this far...
  //======================

  // Assign the value of test_value, and try to work out what the value of
  // sample_result should be, based on the test value and the sample type.
  //-----------------------------------------------------------------------
  globalFieldList.at( QStringLiteral("test_value") )->setFieldIsPresent( true );
  _fieldData.value( QStringLiteral("test_value") )->setData( testVal, true );

  QString sampleType = _fieldData.value( QStringLiteral("sample_type") )->data().toString().toLower();
  if( "Serum" == globalFieldList.sampleType( sampleType ) ) {
    if( testVal < 9.0 )
      _fieldData.value( QStringLiteral("sample_result") )->setData( globalFieldList.herdStatus( "Negative" ), true );
    else
      _fieldData.value( QStringLiteral("sample_result") )->setData( globalFieldList.herdStatus( "NotNegative" ), true );
  }
  else if( "Milk" == globalFieldList.sampleType( sampleType ) ) {
    if( testVal < 9.0 )
      _fieldData.value( QStringLiteral("sample_result") )->setData( globalFieldList.herdStatus( "Negative" ), true );
    else
      _fieldData.value( QStringLiteral("sample_result") )->setData( globalFieldList.herdStatus( "NotNegative" ), true );
  }
  else if( "BulkMilk" == globalFieldList.sampleType( sampleType ) ) {
    if( testVal > 100.0 )
      _fieldData.value( QStringLiteral("sample_result") )->setData( globalFieldList.herdStatus( "NotNegative" ), true );
    else if( testVal > 20.0 )
      _fieldData.value( QStringLiteral("sample_result") )->setData( globalFieldList.herdStatus( "NotNegative" ), true );
    else if( testVal >= 3.0 )
      _fieldData.value( QStringLiteral("sample_result") )->setData( globalFieldList.herdStatus( "Inconclusive" ), true );
    else
      _fieldData.value( QStringLiteral("sample_result") )->setData( globalFieldList.herdStatus( "Negative" ), true );
  }
}


bool CBVDRecord::requirementsOK( const int lineID, CErrorList& msgs ) {
  bool result = true;

  int i;

  for( i = 0; i < globalFieldList.fieldCount(); ++i ) {
    if( globalFieldList.at(i)->isRequired() ) {
      QString fn = globalFieldList.at(i)->name();
      CBVDField* field = this->field( fn );
      if( field->isNull() ) {
        result = false;
        msgs.append( CError::Critical, QStringLiteral( "Line %1: Required field %2 is null." ).arg( lineID ).arg( fn ) );
      }
      else if( !field->isValid() ) {
        result = false;
        msgs.append( CError::Critical, QStringLiteral( "Line %1: Required field %2 is not valid." ).arg( lineID ).arg( fn ) );
      }
    }
  }

  _errMsgs.append( msgs );

  return result;
}


CBVDField* CBVDRecord::field( const QString& fieldName ) {
  if( _fieldData.contains( fieldName ) )
    return _fieldData.value( fieldName );
  else {
    logMsg( QStringLiteral( "Field name '%1' is missing in CBVDRecord::field()" ) );
    return nullptr;
  }
}


QString CBVDRecord::csvColumnHeaders( const bool includeIsValid, const bool excludeParish ) {
  QString result;

  for( int i = 0; i < globalFieldList.fieldCount(); ++i ) {
    QString validFN;
    CBVDBaseField* field;
    if( globalFieldList.at(i)->fieldIsPresent() ) {
      field = globalFieldList.at(i);
      validFN = QStringLiteral( "valid_%1" ).arg( field->name() );

      if( ( "parish" == field->name() ) && excludeParish )
        continue;
      else if( includeIsValid )
        result.append( QStringLiteral( "%1,%2," ).arg( validFN, field->name() ) );
      else
        result.append( QStringLiteral( "%1," ).arg( field->name() ) );
    }
  }

  result.chop( 1 );
  return result;
}


QString CBVDRecord::toCsvText( const QVariant& var ) {
  if( var.isNull() )
    return ( QString() );
  else {
    switch( var.type() ) {
      case QVariant::Bool:
        return QStringLiteral( "%1" ).arg( boolToText( var.toBool() ) );
      case QVariant::Int:
        return QStringLiteral( "%1" ).arg( var.toInt() );
      case QVariant::Double:
        return QStringLiteral( "%1" ).arg( var.toDouble() );
      case QVariant::String:
        return QStringLiteral( "%1" ).arg( csvQuote( var.toString() ) );
      case QVariant::Date:
        return QStringLiteral( "%1" ).arg( var.toDate().toString( QStringLiteral("yyyy-MM-dd") ) );
      default:
        _csvProcessingResult = CProcessor::ERROR_VARIANT_CONVERSION;
        return ( QString() );
    }
  }
}


QString CBVDRecord::csvText(const bool includeIsValid, const bool excludeParish ) {
  QString result;
  _csvProcessingResult = CProcessor::SUCCESS; // Unless changed by one of the following

  QString fieldName;
  QVariant valid;
  QVariant value;
  CBVDField* field;
  for( int i = 0; i < globalFieldList.fieldCount(); ++i ) {
    if( globalFieldList.at(i)->fieldIsPresent() ) {
      fieldName = globalFieldList.at(i)->name();
      field = _fieldData.value( fieldName );
      valid = field->isValid();
      value = field->data();

      if( ( "parish" == field->name() ) && excludeParish )
        continue;
      else if( includeIsValid ) {
        if( value.isNull() )
          result.append( QStringLiteral( "%1,%2," ).arg( toCsvText( QVariant(QVariant::Bool) ), toCsvText( value ) ) );
        else
          result.append( QStringLiteral( "%1,%2," ).arg( toCsvText( valid ), toCsvText( value ) ) );
      }
      else
        result.append( QStringLiteral( "%1," ).arg( toCsvText( value ) ) );
    }
  }

  result.chop( 1 );
  return result;
}


bool CBVDRecord::isValid() {
  return( 0 == _errMsgs.count() );
}


// FIXME: Replace this function by simply checking valid_encrypted_postcode
bool CBVDRecord::postcodeIsNull() {
  bool result = (
    this->field( QStringLiteral("encrypted_postcode") )->data().isNull()
    || this->field( QStringLiteral("encrypted_postcode") )->data().toString().isEmpty()
    || this->field( QStringLiteral("outward_postcode") )->data().isNull()
    || this->field( QStringLiteral("outward_postcode") )->data().toString().isEmpty()
  );

  return result;
}


// FIXME: Replace this function by simply checking valid_encrypted_cph
bool CBVDRecord::herdIsNull() {
  bool result = (
    this->field( QStringLiteral("encrypted_cph") )->data().isNull()
    || this->field( QStringLiteral("encrypted_cph") )->data().toString().isEmpty()
    || this->field( QStringLiteral("county") )->data().isNull()
    || this->field( QStringLiteral("county") )->data().toString().isEmpty()
  );

  if( !this->excludeParish() ) {
    result = (
      result
      || this->field( QStringLiteral("parish") )->data().isNull()
      || this->field( QStringLiteral("parish") )->data().toString().isEmpty()
    );
  }

  return result;
}
