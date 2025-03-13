/*
 * bit_mac_u.h
 *
 * Created: 10.1.2018 11:47:45
 * Author: LH
 */ 

#ifndef _bit_mac_u_
#define _bit_mac_u_ 1

//***********************************************************************************************************************

#define prt(p) (p - 0)
#define ddr(p) (p - 1)
#define pin(p) (p - 2)

//***********************************************************************************************************************

#define PRT(P) (*(&P - 0))
#define DDR(P) (*(&P - 1))
#define PIN(P) (*(&P - 2)) 

//***********************************************************************************************************************

#define _prt(p)((p) >> 0) & 0xFF
#define _bit(p)((p) >> 8) & 0xFF

//***********************************************************************************************************************

#define set_bit(p) (*(&_SFR_IO8(_prt(p))) |= +(1 << _bit(p)))
#define clr_bit(p) (*(&_SFR_IO8(_prt(p))) &= ~(1 << _bit(p)))
#define get_bit(p) (*(&_SFR_IO8(_prt(p))) & (1 << _bit(p)))
#define is_bit_clr(p) get_bit(p) == 0;
#define is_bit_set(p) get_bit(p) != 0;

//***********************************************************************************************************************

#define bit_inp_0(p) clr_bit(ddr(p)); set_bit(prt(p));
#define bit_inp_1(p) clr_bit(ddr(p)); clr_bit(prt(p));
#define bit_out_0(p) set_bit(ddr(p)); clr_bit(prt(p));
#define bit_out_1(p) set_bit(ddr(p)); set_bit(prt(p));
 
//***********************************************************************************************************************

#define ibit(p, b) (_SFR_IO_ADDR(p) + ((b) << 8))

//***********************************************************************************************************************
//release + pullup
//L -> P se provede jako L->Z->P; (01->00->10) = (PRT:DDR->PRT:DDR->PRT:DDR)
//H -> P se provede jako H->P->P; (11->10->10)
//P -> P se provede jako P->P->P; (10->10->10)
//L = log.0; H = log.1; P = pullup (mekka log.1); Z = high imp.

#define rel_bit_x(p) \
	clr_bit(ddr(p)); \
	set_bit(prt(p));	

//***********************************************************************************************************************
//tvrda log.0
//L -> L se provede jako L->L->L; (01->01->01)
//H -> L se provede jako H->H->L; (11->11->01)
//P -> L se provede jako P->H->L; (10->11->01)
//L = log.0; H = log.1; P = pullup (mekka log.1); Z = high imp.

#define	clr_bit_x(p) \
	set_bit(ddr(p)); \
	clr_bit(prt(p));

//***********************************************************************************************************************
//nabiti C + pullup
//L -> H se provede jako L->L->H->P; (01->01->11->10)
//H -> H se provede jako H->H->H->P; (11->11->11->10)
//P -> H se provede jako P->H->H->P; (10->11->11->10)
//L = log.0; H = log.1; P = pullup (mekka log.1); Z = high imp.

#define	set_bit_x(p) \
	set_bit(ddr(p)); \
	set_bit(prt(p)); \
	clr_bit(ddr(p)); 

//***********************************************************************************************************************

#endif