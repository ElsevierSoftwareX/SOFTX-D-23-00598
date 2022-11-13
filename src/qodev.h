/**************************************************************************
* @file qodev.h
* @author Javier Osca
* @author Jiri Vala
*
* @copyright Copyright � 2022 National University of Ireland Maynooth, Maynooth University. All rights reserved.
*            The contents of this file are subject to the licence terms detailed in LICENCE.TXT available in the
*            root directory of this source tree. Use of the source code in this file is only permitted under the
*            terms of the licence in LICENCE.TXT.
*
* @title Quantum optical device library
*
***************************************************************************/


/***********************************************************************************
 QUICK GLANCE INDEX
************************************************************************************
class qodev{
    // Public methods
    // Management methods
    qodev(int i_nph, int i_nch);                                                        // Creates a metacircuit
    qodev(int i_nph, int i_nch, int i_nm);                                              // Creates a polarized metacircuit
    qodev(int i_nph, int i_nch, int i_nm, int i_ns, int clock, char ckind);                                        //  Creates a physical metacircuit
    qodev(int i_nph, int i_nch, int i_nm, int i_ns, int clock, int i_R, bool loss, char ckind, int i_maxket);      //  Creates a physical metacircuit with physical detectors
    ~qodev();                                                                           //  Destroys a metacircuit
    void reset();                                                                       //  Clears a metacircuit

    // Initial state definition
    int add_photons(int N, int ch);                                                     //  Add photons to a metacircuit
    int add_photons(int N, int ch, int P, double t, double f, double w);                // Add photons with physical description of the photon shape
    int add_Bell (int ch1, int ch2, char kind);                                         //  Add photons to a metacircuit as generated by an ideal Bell state. (Path encoding)
    int add_BellP(int ch1, int ch2, char kind);                                         //  Add photons to a metacircuit as generated by an ideal Bell state. (Polarization encoding)
    int add_Bell(int ch1, int ch2, double phi, char kind, double t1, double f1, double w1, double t2, double f2, double w2); //  Add photons to a metacircuit as generated by an unsynchronized Bell state. (Path encoding)
    int add_BellP(int ch1, int ch2, double phi, char kind, double t1, double f1, double w1, double t2, double f2, double w2);//  Add photons to a metacircuit as generated by an unsynchronized Bell state. (Polarization encoding)
    int add_QD(int ch1, int ch2,double i_t1, double i_f1, double i_w1, double i_t2, double i_f2, double i_w2, int PW, double S, double k, double tss, double thv);                                                  //  Add photons to a metacircuit as generated by a QD
    int add_QD(int ch1, int ch2,double i_t1, double i_f1, double i_w1, double i_t2, double i_f2, double i_w2, int PW, double S, double k, double tss, double thv, double T2);                                       //  Add photons to a metacircuit as generated by a QD
    int add_QD(int ch1, int ch2,double i_t1, double i_tp1, double i_f1, double i_w1, double i_t2, double i_tp2, double i_f2, double i_w2, int PW, double S, double k, double tss, double thv, double T2, int rand); //  Add photons to a metacircuit as generated by a QD
    state *input();                                                                     //  Returns the internal photon state.
    qocircuit *circuit();                                                               //  Returns the internal circuit object.
    void repack(veci ipack);                                                            // Changed packets Gram-Schmidt order
    double emitted_vis(int i,int j);                                                    // Probability of two wave packets given by def_packet to overlap.
    void prnt_packets();                                                                // Prints metacircuit packet configuration

    // Circuit elements
    //      Basic elements
    void random_circuit();                                                              // Adds a random circuit
    void NSX(int i_ch1, int i_ch2, int i_ch3);                                          // Adds a built-in NSX circuit
    void beamsplitter(int i_ch1, int i_ch2, double theta, double phi);                  // Adds a ideal beamsplitter to the circuit
    void dielectric(int i_ch1, int i_ch2, cmplx t, cmplx r);                            // Adds a physical dieletric beamsplitter
    void MMI2(int i_ch1, int ch2);                                                      // Adds a 2x2 ideal MMI
    void rewire(int i_ch1,int i_ch2);                                                   // Adds a swap gate between two channels
    void phase_shifter(int i_ch, double phi);                                           // Adds a phase_shifter to the circuit
    void loss(int i_ch, double l);                                                      // Adds a lossy medium
    void dispersion(int i_ch, double dt);                                               // Adds a phase shift to the photons in a channel depending on the optical path and photon frequency
    void dispersion(int i_ch, int P, double dt);                                        // Adds a phase shift to the photons with polariation P in a channel depending on the optical path and photon frequency
    void delay(int ch, double dt);                                                      // Adds a delay using the packet definition given by def_packet


    //      Polarization elements
    void rotator(int i_ch, double d_theta, double d_phi);                               // Adds a rotator to the circuit.
    void polbeamsplitter(int i_ch1, int i_ch2, int P);                                  // Adds a polarizing beamsplitter
    void half(int i_ch, double alpha);                                                  // Adds a half-waveplate
    void quarter(int i_ch, double alpha);                                               // Adds a quarter-waveplate

    //      Detection elements
    void ignore(int i_ch);                                                              // Flags a channel to be ignored
    void detector(int i_ch);                                                            // Adds a detector
    void detector(int i_ch, int cond);                                                  // Adds a conditional detection
    void detector(int i_ch, int cond, double eff, double blnk, double gamma);           // Adds a general physical detector
    void noise(double stdev2);                                                          // Adds noise to the output
    state *apply_condition(state *input);                                               // Apply a single-ket post selection to ideal circuits.

protected:
    // Auxiliary methods
    void create_qodev(int i_level, int i_maxket, int i_np);                             //  Auxiliary private method to create a metacircuit
    void send2circuit();                                                                // Send photons to the circuit
};
***********************************************************************************/

#include "state.h"


/** @defgroup QODev Device
 *  Quantum optical device that consist in a circuit and a photon abstraction of its initial state.
 */

/** \class qodev
*   \brief Contains all the information
*          to create and manipulate a quantum optical device.
*   \author Javier Osca
*   \author Jiri Vala
*
*   \copyright Copyright &copy; 2022 National University of Ireland Maynooth, Maynooth University. All rights reserved. <br>
*              The contents and use of this document and the related code are subject to the licence terms detailed in <a  href="../assets/LICENCE.TXT"> LICENCE.txt </a>.
*
*   @ingroup PBunch
*
*/
class qodev{
public:
    int        npack;       ///< Number of wave-packets
    matd       pack_list;   ///< Wave-packet definitions
    state     *inpt;        ///< Photon state
    qocircuit *circ;        ///< Quantum optical circuit

    // Public methods
    // Management methods
    /** @defgroup QODev_management Device management
    *   @ingroup QODev
    *   Creation and management of quantum optical devices
    */

    /**
    *  Creates a quantum optical device.
    *
    *  @param int i_nph   Maximum number of photons.
    *  @param int i_nch   Number of channels.
    *  @ingroup QODev_management
    */
    qodev(int i_nph, int i_nch);
    /**
    *  Creates a quantum optical device.
    *
    *  @param int i_nph   Maximum number of photons.
    *  @param int i_nch   Number of channels.
    *  @param int i_nm    Number of modes
    *  @ingroup QODev_management
    */
    qodev(int i_nph, int i_nch, int i_nm);
    /**
    *  Creates a quantum optical device.
    *
    *  @param int i_nph  Maximum number of photons.
    *  @param int i_nch  Number of channels.
    *  @param int i_nm   Number of modes.
    *  @param int i_ns   Number of packets.
    *  @param int clock  Detector behavior configuration.<br>
    *           - 0: No clock. The detectors behave as counters. <br>
    *           - 1: Clock. The detectors are able distinguish arrival times but they are blind to frequency. <br>
    *           - 2: Full.  The detectors are able to distinguish arrival times and frequency of the outgoing photons. <br>
    *           - 3: Manual mode.  Like 1. But the user is fully responsible of the packet definitions in order to obtain the correct calculations. <br>
    *                 <b>Warning!</b> This is an advanced configuration. It is not recommended unless the user has knowledge on how SOQCS internally works. <br>
    *
    *  @param char ckind Packet model 'G': Gaussian/ 'E': Exponential
    *  @ingroup QODev_management
    */
    qodev(int i_nph, int i_nch, int i_nm, int i_ns, int clock, char ckind);
    /**
    *  Creates a quantum optical device.
    *
    *  @param int i_nph  Maximum number of photons.
    *  @param int i_nch  Number of channels.
    *  @param int i_nm   Number of modes.
    *  @param int i_ns   Number of packets.
    *  @param int clock  Detector behavior configuration.<br>
    *           - 0: No clock. The detectors behave as counters. <br>
    *           - 1: Clock. The detectors are able distinguish arrival times but they are blind to frequency. <br>
    *           - 2: Full.  The detectors are able to distinguish arrival times and frequency of the outgoing photons. <br>
    *           - 3: Manual mode.  Like 1. But the user is fully responsible of the packet definitions in order to obtain the correct calculations. <br>
    *                 <b>Warning!</b> This is an advanced configuration. It is not recommended unless the user has knowledge on how SOQCS internally works. <br>
    *
    *  @param int i_R    Number of iterations in the calculation of detector dead time and dark counts effects.
    *  @param bool loss  Are the losses calculated explicitly? True/False.
    *  @param char ckind Packet model 'G': Gaussian/ 'E': Exponential
    *  @param int i_maxket  Maximum number of kets in the initial state. (Internal memory).
    *  @ingroup QODev_management
    */
    qodev(int i_nph, int i_nch, int i_nm, int i_ns, int clock, int i_R, bool loss, char ckind, int i_maxket);
    /**
    *  Destroys a  quantum optical device object.
    *
    *  @ingroup QODev_management
    */
    ~qodev();
    /**
    *  Clears a quantum optical device object.
    *
    *  @ingroup QODev_management
    */
    void reset();
    /**
    *  Concatenates two quantum optical devices with some limitations.<br>
    *  All the input photons have to be defined in the first circuit and all detectors in the last.
    *  Circuits with delays cannot be concatenated either.
    *
    *  @param qodev* qoc Device to be concatenated.
    *  @ingroup QODev_management
    */
    void concatenate(qodev *dev);

    // Initial state definition
    /** @defgroup QODev_initial Device initial state
    *   @ingroup QODev
    *   Operations to configure the initial state of a quantum optical device.
    */
    /**
    *  Adds N photons to the quantum optical device.
    *
    *  @param int N Number of photons to be added.
    *  @param int ch Channel where the photons are added.
    *  @return 0 if success -1 if an error happened.
    *  @ingroup QODev_initial
    */
    int add_photons(int N, int ch);
    /**
    *  Adds N photons to the quantum optical device.
    *
    *  @param int N Number of photons to be added.
    *  @param int ch Channel where the photons are added.
    *  @param int P Polarization of the photons.
    *  @param double t Emission time of the photons.
    *  @param double f Emission frequency of the photons.
    *  @param double w Emission width/decay time of the photons depending on the packet shape model.
    *  @return 0 if success -1 if an error happened.ss
    *  @ingroup QODev_initial
    */
    int add_photons(int N, int ch, int P, double t, double f, double w);
    /**
    *  Adds an ideal Bell state to the input of the circuit. (Path encoding).
    *
    *  @param int ch1 Channel where a photon is added.
    *  @param int ch2 Channel where a photon is added.
    *  @param char kind  Which of the four bell states is created. <br>
    *  @code
       '+'=|00> + |11>
       '-'=|00> - |11>
       'p'=|01> + |10>
       'm'=|01> - |10>
    *  @endcode
    *  @return 0 if success -1 if an error happened.
    *  @ingroup QODev_initial
    */
    int add_Bell (int ch1, int ch2, char kind);
    /**
    *  Adds an ideal Bell state to the input of the circuit. (Polarization encoding).
    *
    *  @param int ch1 Channel where a photon is added.
    *  @param int ch2 Channel where a photon is added.
    *  @param char kind  Which of the four bell states is created. <br>
    *  @code
       '+'=|HH> + |VV>
       '-'=|HH> - |VV>
       'p'=|HV> + |VH>
       'm'=|HV> - |VH>
    *  @endcode
    *  @return 0 if success -1 if an error happened.
    *  @ingroup QODev_initial
    */
    int add_BellP(int ch1, int ch2, char kind);
    /**
    *  Adds an ideal Bell state to the input of the circuit. (Path encoding).
    *
    *  @param int ch1 Channel where a photon is added.
    *  @param int ch2 Channel where a photon is added.
    *  @param double phi  Phase difference between the first and second ket in the definition of the Bell state.
    *  @code
        | A, B > + exp(-i phi)| C, D >
    *  @endcode
    *  @param char kind  Which of the four bell states is created. <br>
    *  @code
       '+'=|00> + |11>
       '-'=|00> - |11>
       'p'=|01> + |10>
       'm'=|01> - |10>
    *  @endcode
    *  @param double t1 Photon in channel 1 emission time
    *  @param double f1 Photon in channel 1 emission frequency.
    *  @param double w1 Photon in channel 1 emission width/decay time
    *  @param double t2 Photon in channel 2 emission time
    *  @param double f2 Photon in channel 2 emission frequency.
    *  @param double w2 Photon in channel 2 emission width/decay time
    *  @return 0 if success -1 if an error happened.
    *  @ingroup QODev_initial
    */
    int add_Bell(int ch1, int ch2, char kind, double phi, double t1, double f1, double w1, double t2, double f2, double w2);
    /**
    *  Adds an ideal Bell state to the input of the circuit. (Polarization encoding).
    *
    *  @param int ch1 Channel where a photon is added.
    *  @param int ch2 Channel where a photon is added.
    *  @param double phi  Phase difference between the first and second ket in the definition of the Bell state.
    *  @code
        | A, B > + exp(-i phi)| C, D >
    *  @endcode
    *  @param char kind  Which of the four bell states is created. <br>
    *  @code
       '+'=|HH> + |VV>
       '-'=|HH> - |VV>
       'p'=|HV> + |VH>
       'm'=|HV> - |VH>
    *  @endcode
    *  @param double t1 Photon in channel 1 emission time
    *  @param double f1 Photon in channel 1 emission frequency.
    *  @param double w1 Photon in channel 1 emission width/decay time
    *  @param double t2 Photon in channel 2 emission time
    *  @param double f2 Photon in channel 2 emission frequency.
    *  @param double w2 Photon in channel 2 emission width/decay time
    *  @return 0 if success -1 if an error happened.
    *  @ingroup QODev_initial
    */
    int add_BellP(int ch1, int ch2, char kind, double phi, double t1, double f1, double w1, double t2, double f2, double w2);
    /**
    *  Adds a pair of photons to the input of a circuit as if they where emitted by a QD. <br>
    *  <b> Important:</b> No cascade uncertainty simulation is carried by this method. The emission times of the photons are taken as the ones provided in the parameters.
    *
    *  @param int ch1 Channel where the XX photon is added
    *  @param int ch2 Channel where the X  photon is added
    *  @param double t1 XX Photon emission time
    *  @param double f1 XX Photon emission frequency.
    *  @param double w1 XX Photon emission width/decay time
    *  @param double t2 X  Photon emission time
    *  @param double f2 X  Photon emission frequency.
    *  @param double w2 X  Photon emission width/decay time
    *  @param int PW Dow we consider the frequency shift due FSS 0='No'/1='Yes'
    *  @param double S S/hbar Fine Structure Splitting (FSS) constant.
    *  @param double k Fraction of the emitted photon pairs which originate from a XX-X cascade over a background of noise.
    *  @param double tss Characteristic time of spin-scattering tss.
    *  @param double thv Characteristic time of cross-dephasing thv.
    *  @return 0 if success -1 if an error happened.
    *  @ingroup QODev_initial
    */
    int add_QD(int ch1, int ch2,double i_t1, double i_f1, double i_w1, double i_t2, double i_f2, double i_w2, int PW, double S, double k, double tss, double thv);
    /**
    *  Adds a pair of photons to the input of a circuit as if they where emitted by a QD in a XX-X cascade. <br>
    *  <b> Important:</b> The exciton photon is emitted after the bi-exciton photon with an uncertainty exponentially distributed with a characteristic decay tx. <br>
    *  In this case, t2 is a constant bias that can be set-up explicitly to be added to the calculated emission time of the exciton (physically is usually zero).
    *  Note that that pure dephasing effects are also considered introducing a T2>0 time.
    *
    *  @param int ch1 Channel where the XX photon is added
    *  @param int ch2 Channel where the X  photon is added
    *  @param double t1 XX Photon emission time
    *  @param double f1 XX Photon emission frequency.
    *  @param double w1 XX Photon emission width/decay time
    *  @param double t2 X  Photon emission time manual bias (physically uses to be zero).
    *  @param double f2 X  Photon emission frequency.
    *  @param double w2 X  Photon emission width/decay time
    *  @param int PW Dow we consider the frequency shift due FSS 0='No'/1='Yes'
    *  @param double S S/hbar Fine Structure Splitting (FSS) constant.
    *  @param double k Fraction of the emitted photon pairs which originate from a XX-X cascade over a background of noise.
    *  @param double tss Characteristic time of spin-scattering tss.
    *  @param double thv Characteristic time of cross-dephasing thv.
    *  @param double T2 Characteristic time of pure dephasing
    *  @return 0 if success -1 if an error happened.
    *  @ingroup QODev_initial
    */
    int add_QD(int ch1, int ch2,double i_t1, double i_f1, double i_w1, double i_t2, double i_f2, double i_w2, int PW, double S, double k, double tss, double thv, double T2);
    /**
    *  Adds a pair of photons to the input of a circuit as if they where emitted by a QD in a XX-X cascade. In this case the emission pattern and phases are configured explicitly.
    *
    *  @param int ch1 Channel where the XX photon is added
    *  @param int ch2 Channel where the X  photon is added
    *  @param double t1  XX Photon emission time
    *  @param double tp1 XX Photon phase time
    *  @param double f1  XX Photon emission frequency.
    *  @param double w1  XX Photon emission width/decay time
    *  @param double t2  X  Photon emission time
    *  @param double tp2 X Photon phase time
    *  @param double f2  X  Photon emission frequency.
    *  @param double w2  X  Photon emission width/decay time
    *  @param int PW Do we consider the frequency shift due FSS 0='No'/1='Yes'
    *  @param double S S/hbar Fine Structure Splitting (FSS) constant.
    *  @param double k Fraction of the emitted photon pairs which originate from a XX-X cascade over a background of noise.
    *  @param double tss Characteristic time of spin-scattering tss.
    *  @param double thv Characteristic time of cross-dephasing thv.
    *  @param double T2 Characteristic time of pure dephasing
    *  @param int rand Kind of emission pattern
    *  - 0=XX-X-G cascade fully automatic emission calculation. The exciton phase time is configured equal to its randomly generated emission time.
    *  - 1=XX-X-G cascade automatic emission and phase calculation. Same than 0 but the exciton phase time is randomly generated separately from a distribution parametrized by T2.
    *  - 2=XX-X-G cascade semi-automatic calculation. The exciton phase time is randomly calculated using T2 like in 1 but the exciton emission time is set manually.
    *
    *  In all rand configurations the phase origin times are set up manually as tp1 and tp2.
    *  @return 0 if success -1 if an error happened.
    *  @ingroup QODev_initial
    */
    int add_QD(int ch1, int ch2,double i_t1, double i_tp1, double i_f1, double i_w1, double i_t2, double i_tp2, double i_f2, double i_w2, int PW, double S, double k, double tss, double thv, double T2, int rand);
    /**
    *  Returns a copy of the photon initial state.
    *
    *  @return The state of the defined photons.
    *  @ingroup QODev_initial
    */
    state *input();
    /**
    *  Returns a quantum optical circuit equivalent to the quantum device except for the initial conditions that are not defined in a circuit.
    *
    *  @return The circuit of the quantum device.
    *  @ingroup QODev_initial
    */
    qocircuit *circuit();
    /**
    *  Changes the Gram-Schmidt orthonormalization order.
    *
    *  @param veci ipack  Vector with the preferred packet order.
    *  @ingroup QODev_initial
    */
    int repack(veci ipack);
    /**
    *  Overlapping probability of two wave packets
    *
    *  @param int i  Packet i number.
    *  @param int j  Packet j number.
    *  @return Probability of overlapping.
    *  @ingroup QODev_initial
    */
    double emitted_vis(int i,int j);
    /**
    *  Prints the packet configuration of the quantum optical device.
    *
    *  @ingroup QODev_initial
    */
    void prnt_packets();


    // Circuit elements
    /** @defgroup QODev_Circuit Device elements
    *   @ingroup QODev
    *   List of elements that can be added into a quantum optical device.
    */
    //     Basic elements
    /** @defgroup QODev_Circuit_basic Basic device elements
    *   @ingroup QODev_Circuit
    *   List of basic device elements.
    */

    /**
    *  Adds a circuit defined by a random unitary matrix.
    *
    *  @ingroup QODev_Circuit_basic
    */
    void random_circuit();
    /**
    *  Adds a NSX circuit element. Post-selection still has to be carried out to obtain the proper functionality.
    *
    *  @param int i_ch1     NSX input channel 1.
    *  @param int i_ch2     NSX input channel 2.
    *  @param int i_ch3     NSX input channel 3.
    *  @ingroup QODev_Circuit_basic
    */
    void NSX(int i_ch1, int i_ch2, int i_ch3);
    /**
    *  Adds a beamsplitter to the device attached to channels i_ch1 and i_ch2.
    *
    *  @param int i_ch1     Beamsplitter input channel 1.
    *  @param int i_ch2     Beamsplitter input channel 2.
    *  @param double theta  Angle theta in degrees.
    *  @param double phi    Angle phi in degrees.
    *  @ingroup QODev_Circuit_basic
    */
    void beamsplitter(int i_ch1, int i_ch2, double theta, double phi);
    /**
    *  Adds a dieletric film attached to channels i_ch1 and i_ch2
    *  It may also work as a dieletric beamsplitter.
    *
    *  @param int i_ch1     Dielectric input channel 1.
    *  @param int i_ch2     Dielectric input channel 2.
    *  @param cmplx t       Transmission amplitude of probability.
    *  @param cmplx r       Reflection amplitude of probability.
    *  @ingroup QODev_Circuit_basic
    */
    void dielectric(int i_ch1, int i_ch2, cmplx t, cmplx r);
    /**
    *  Adds a 2x2 MMI device to the device attached to channels i_ch1 and i_ch2.
    *
    *  @param int i_ch1     MMI input channel 1.
    *  @param int i_ch2     MMI input channel 2.
    *  @ingroup QODev_Circuit_basic
    */
    void MMI2(int i_ch1, int i_ch2);
    /**
    *  Adds a swap gate between two channels.
    *
    *  @param int i_ch1     Channel 1.
    *  @param int i_ch2     Channel 2.
    *  @ingroup QODev_Circuit_basic
    */
    void rewire(int i_ch1,int i_ch2);
    /**
    *  Adds a phase shifter to the device in channel i_ch.
    *
    *  @param int i_ch      Phase shifter input channel.
    *  @param double phi    Angle phi in degrees.
    *  @ingroup QODev_Circuit_basic
    */
    void phase_shifter(int i_ch, double phi);
    /**
    *  Adds a lossy medium with loss probability l to the device in channel i_ch.
    *
    *  @param int i_ch      Loss medium input channel.
    *  @param double l      Loss probability.
    *  @ingroup QODev_Circuit_basic
    */
    void loss(int i_ch, double l);
    /**
    *  Adds a phase shift to the photons in channel i_ch depending on
    *  their frequency and optical path dt.
    *
    *  @param int i_ch      Phase shifter input channel.
    *  @param double dt     Optical path given in time units.
    *  @ingroup QODev_Circuit_basic
    */
    void dispersion(int i_ch, double dt);
    /**
    *  Adds a phase shift to the photons in channel i_ch depending on
    *  their frequency and optical path dt provided the photons match the given polarization P.
    *
    *  @param int i_ch      Phase shifter input channel.
    *  @param int P         Polarization required to the photons to apply the phase shift.
    *  @param double dt     Optical path given in time units.
    *  @ingroup QODev_Circuit_basic
    */
    void dispersion(int i_ch, int P, double dt);
    /**
    *  Increases the optical path of a channel by a quantity dt.
    *
    *  @param int  i_ch  Channel where the delay is introduced.
    *  @param double dt  Delay time introduced in the channel.
    *  @ingroup QODev_Circuit_basic
    * \xrefitem know "KnowIss" "Known Issues" Delay can not be called if the circuit is configured to calculate explicit losses (flag loss=true in qocircuit constructor).
    */
    void delay(int ch, double dt);


    //     Polarization elements
    /** @defgroup QODev_Circuit_polar Device polarization elements
    *   @ingroup QODev_Circuit
    *   List of circuit elements that act on photon polarization
    */
    /**
    *  Adds a polarization rotation device to the device attached to the channel i_ch.
    *
    *  @param int i_ch      Rotator input channel.
    *  @param double theta  Angle theta in degrees.
    *  @param double phi    Angle phi in degrees.
    *  @ingroup QODev_Circuit_polar
    */
    void rotator(int i_ch, double d_theta, double d_phi);
    /**
    *  Adds a polarized beamsplitter attached to channels i_ch1 and i_ch2.
    *
    *  @param int i_ch1     Beamsplitter input channel 1.
    *  @param int i_ch2     Beamsplitter input channel 2.
    *  @param int P         Polarization to which the beamsplitter is sensitive.
    *  @ingroup QODev_Circuit_polar
    */
    void polbeamsplitter(int i_ch1, int i_ch2, int pol);
    /**
    *  Adds a half waveplate attached to channels i_ch1 and i_ch2.
    *
    *  @param int i_ch1      Waveplate input channel 1.
    *  @param int i_ch2      Waveplate input channel 2.
    *  @param double d_alpha Rotation angle in degrees.
    *  @ingroup QODev_Circuit_polar
    */
    void half(int i_ch, double alpha);
    /**
    *  Adds a quarter waveplate attached to channels i_ch1 and i_ch2.
    *
    *  @param int i_ch1      Waveplate input channel 1.
    *  @param int i_ch2      Waveplate input channel 2.
    *  @param double d_alpha Rotation angle in degrees.
    *  @ingroup QODev_Circuit_polar
    */
    void quarter(int i_ch, double alpha);


    //      Detection elements
    /** @defgroup QODev_Circuit_detector Detectors
    *   @ingroup QODev_Circuit
    *   List of circuit elements to define the circuit detectors.
    */
    /**
    *  Flags the channel to be ignored in the outcome calculations. No detector is placed in this channel.
    *
    *  @param int i_ch      Detector channel.
    *  @ingroup QODev_Circuit_detector
    */
    void ignore(int i_ch);
    /**
    *  Adds a detector to a channel of the circuit.
    *
    *  @param int i_ch      Detector channel.
    *  @ingroup QODev_Circuit_detector
    */
    void detector(int i_ch);
    /**
    *  Adds a detector to a channel of the device.
    *
    *  @param int i_ch Detector channel.
    *  @ingroup QODev_Circuit_detector
    */
    void detector(int i_ch, int cond);
    /**
    *  Adds a general physical detector to a channel of the device.
    *
    *  @param int i_ch      Detector channel.
    *  @param int cond      Detection condition.<br>
    *       cond>=0:  Readings in the remaining channels are considered only by calculations in probability bins and density matrices if the number of photons in this channel is equal to cond.<br>
    *       cond=-1:  There is no condition and works as a normal detector.<br>
    *       cond=-2:  The channel is ignored by outcome calculations in probability bins and density matrices.<br>
    *  @param double eff    Efficiency of the detector.
    *  @param duble  blnk   Ratio of time in which the detector is inactive due other detections.
    *  @param double gamma  Average rate of dark counts in this channel.
    *  @ingroup QODev_Circuit_detector
    */
    void detector(int i_ch, int cond, double eff, double blnk, double gamma);
    /**
    *  Adds Gaussian white noise to the output.
    *
    *  @param stdev2 Dispersion of the Gaussian distribution.
    *  @ingroup QODev_Circuit_detector
    */
    void noise(double stdev2);

    /**
    *  Apply the post-selection condition defined by the detectors in an ideal circuit to a state. <br>
    *  <b> Warning!</b>. This can be only applied to ideal devices nm=1 and nd=1 where detector naturally define a single ket projector. Otherwise the conditions lead to a density matrix output and
    *  there are other tools available in SOQCS for that purpose. Note that ignored channels are not transcribed but that may lead to collision between otherwise different terms.
    *
    * @param state *input Input state to be post-selected applying the detector conditions.
    * @param ignore Consider ignored channels (false=No/true=Yes)
    * @return Post-selected state.
    * @ingroup QODev_Circuit_detector
    */
    state *apply_condition(state *input, bool ignore);
protected:
    // Auxiliary methods
    /** @defgroup Aux_QODev Device auxiliary methods
    *   @ingroup QODev
    *   List of auxiliary  quantum optical circuit operations
    */

    /**
    *  Auxiliary method to create a quantum optical device.<br>
    *  <b> Intended for internal use of the library. </b>
    *
    *  @param int i_level  Number of levels.
    *  @param int i_maxket Maximum number of kets in the initial state. (Internal memory).
    *  @param int i_np     Number of packets.
    *  @ingroup Aux_QODev
    */

    void create_qodev(int i_level, int i_maxket, int i_np);
    /**
    *  Send photons to the circuit (Photon emission). <br>
    *  <b> Intended for internal use of the library. </b>
    *
    *  @ingroup Aux_QODev
    */
    void send2circuit();
};
