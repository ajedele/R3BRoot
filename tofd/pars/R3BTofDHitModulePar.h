/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include "FairParGenericSet.h"

#define NCHMAX 5000

class FairParamList;

/**
 * Container for time calibration of a single module. This class is used for
 * storage of Tofd hit parameter
 */

class R3BTofDHitModulePar : public FairParGenericSet
{
  public:
    /**
     * Standard constructor.
     * @param name a name of container.
     * @param title a title of container.
     * @param context context/purpose for parameters and conditions.
     * @param own class ownership, if flag is kTRUE FairDB has the par.
     */
    R3BTofDHitModulePar(const char* name = "TofdHitModulePar",
                        const char* title = "TOFd hit calibration of a module",
                        const char* context = "TestDefaultContext",
                        bool own = kTRUE);

    /**
     * Destructor.
     * Frees the memory allocated by the object.
     */
    virtual ~R3BTofDHitModulePar(void);

    /**
     * A method to reset the parameter values. Sets all parameters to 0.
     */
    void clear(void);

    /**
     * A method to write parameters using RuntimeDB.
     * @param list a list of parameters.
     */
    void putParams(FairParamList* list);

    /**
     * A method to read parameters using RuntimeDB.
     * @param list a list of parameters.
     * @return kTRUE if successfull, else kFALSE.
     */
    bool getParams(FairParamList* list);

    /**
     * A method to print value of parameters to the standard
     * output using FairLogger.
     */
    void printParams();

    /** Accessor functions **/
    int GetPlane() const { return fPlane; }
    int GetPaddle() const { return fPaddle; }
    double GetSync() const { return fSync; }
    double GetOffset1() const { return fOffset1; }
    double GetOffset2() const { return fOffset2; }
    double GetToTOffset1() const { return fToTOffset1; }
    double GetToTOffset2() const { return fToTOffset2; }
    double GetVeff() const { return fVeff; }
    double GetLambda() const { return fLambda; }
    double GetPar1a() const { return fPar1a; }
    double GetPar1b() const { return fPar1b; }
    double GetPar1c() const { return fPar1c; }
    double GetPar1d() const { return fPar1d; }
    double GetPar2a() const { return fPar2a; }
    double GetPar2b() const { return fPar2b; }
    double GetPar2c() const { return fPar2c; }
    double GetPar2d() const { return fPar2d; }
    double GetPola() const { return fPola; }
    double GetPolb() const { return fPolb; }
    double GetPolc() const { return fPolc; }
    double GetPold() const { return fPold; }
    double GetPar1za() const { return fPar1za; }
    double GetPar1zb() const { return fPar1zb; }
    double GetPar1zc() const { return fPar1zc; }
    double GetPar1Walk() const { return fPar1walk; }
    double GetPar2Walk() const { return fPar2walk; }
    double GetPar3Walk() const { return fPar3walk; }
    double GetPar4Walk() const { return fPar4walk; }
    double GetPar5Walk() const { return fPar5walk; }
    void SetPlane(int i) { fPlane = i; }
    void SetPaddle(int i) { fPaddle = i; }
    void SetSync(double t) { fSync = t; }
    void SetOffset1(double t) { fOffset1 = t; }
    void SetOffset2(double t) { fOffset2 = t; }
    void SetToTOffset1(double t) { fToTOffset1 = t; }
    void SetToTOffset2(double t) { fToTOffset2 = t; }
    void SetVeff(double v) { fVeff = v; }
    void SetLambda(double v) { fLambda = v; }
    void SetPar1a(double par1a) { fPar1a = par1a; }
    void SetPar1b(double par1b) { fPar1b = par1b; }
    void SetPar1c(double par1c) { fPar1c = par1c; }
    void SetPar1d(double par1d) { fPar1d = par1d; }
    void SetPar2a(double par2a) { fPar2a = par2a; }
    void SetPar2b(double par2b) { fPar2b = par2b; }
    void SetPar2c(double par2c) { fPar2c = par2c; }
    void SetPar2d(double par2d) { fPar2d = par2d; }
    void SetPola(double pola) { fPola = pola; }
    void SetPolb(double polb) { fPolb = polb; }
    void SetPolc(double polc) { fPolc = polc; }
    void SetPold(double pold) { fPold = pold; }
    void SetPar1za(double par1za) { fPar1za = par1za; }
    void SetPar1zb(double par1zb) { fPar1zb = par1zb; }
    void SetPar1zc(double par1zc) { fPar1zc = par1zc; }
    void SetPar1Walk(double par1Walk) { fPar1walk = par1Walk; }
    void SetPar2Walk(double par2Walk) { fPar2walk = par2Walk; }
    void SetPar3Walk(double par3Walk) { fPar3walk = par3Walk; }
    void SetPar4Walk(double par4Walk) { fPar4walk = par4Walk; }
    void SetPar5Walk(double par5Walk) { fPar5walk = par5Walk; }

  private:
    int fPlane;                            /**< Index of a plane. */
    int fPaddle;                           /**< Index of a paddle. */
    double fOffset1;                       /**< time offset of PM1 */
    double fOffset2;                       /**< time offset of PM2 */
    double fToTOffset1;                    /**< ToT offset of PM1 */
    double fToTOffset2;                    /**< ToT offset of PM2 */
    double fVeff;                          /**< effective velocity of light in paddle for position determination */
    double fLambda;                        /**< light attenuation in paddle for position determination */
    double fSync;                          /**< time offset between paddles */
    double fPar1a, fPar1b, fPar1c, fPar1d; /**< double exponential parameters for position dependent charge */
    double fPar2a, fPar2b, fPar2c, fPar2d; /**< double exponential parameters for position dependent charge */
    double fPola, fPolb, fPolc, fPold;     /**< polynomial parameters for position dependent charge */
    double fPar1za, fPar1zb, fPar1zc, fPar1zd;                    /**< quench correction parameters */
    double fPar1walk, fPar2walk, fPar3walk, fPar4walk, fPar5walk; /**< walk correction parameters */

  public:
    ClassDef(R3BTofDHitModulePar, 3);
};
