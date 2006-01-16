/*****************************************************************************
 * Free42 -- a free HP-42S calculator clone
 * Copyright (C) 2004-2006  Thomas Okken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *****************************************************************************/

#include <stdlib.h>

#include "core_commands2.h"
#include "core_commands3.h"
#include "core_commands4.h"
#include "core_decimal.h"
#include "core_display.h"
#include "core_helpers.h"
#include "core_linalg.h"
#include "core_main.h"
#include "core_math.h"
#include "core_variables.h"

static int mappable_acosh_r(double x, double *y) COMMANDS3_SECT;
static int mappable_acosh_r(double x, double *y) {
    if (x >= 1) {
	*y = acosh(x);
	return ERR_NONE;
    } else
	return ERR_INVALID_DATA;
}

static int mappable_acosh_c(double xre, double xim, double *yre, double *yim)
								COMMANDS3_SECT;
static int mappable_acosh_c(double xre, double xim, double *yre, double *yim) {
    return math_asinh_acosh(xre, xim, yre, yim, 0);
}

int docmd_acosh(arg_struct *arg) {
    vartype *v;
    if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else if (reg_x->type == TYPE_REAL) {
	double x = ((vartype_real *) reg_x)->x;
	if (x < 1) {
	    if (flags.f.real_result_only)
		return ERR_INVALID_DATA;
	    else {
		double re, im;
		int err = math_asinh_acosh(x, 0, &re, &im, 0);
		if (err != ERR_NONE)
		    return err;
		v = new_complex(re, im);
	    }
	} else
	    v = new_real(acosh(x));
	if (v == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
    } else {
	int err = map_unary(reg_x, &v, mappable_acosh_r, mappable_acosh_c);
	if (err != ERR_NONE)
	    return err;
    }
    unary_result(v);
    return ERR_NONE;
}

int docmd_aleng(arg_struct *arg) {
    vartype *v = new_real(reg_alpha_length);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    recall_result(v);
    return ERR_NONE;
}

int docmd_allsigma(arg_struct *arg) {
    flags.f.all_sigma = 1;
    return ERR_NONE;
}

int docmd_and(arg_struct *arg) {
    int8 x, y;
    int err;
    vartype *v;
    if ((err = get_base_param(reg_x, &x)) != ERR_NONE)
	return err;
    if ((err = get_base_param(reg_y, &y)) != ERR_NONE)
	return err;
    v = new_real((double) (x & y));
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    binary_result(v);
    return ERR_NONE;
}

int docmd_aoff(arg_struct *arg) {
    flags.f.alpha_mode = 0;
    set_menu(MENULEVEL_ALPHA, MENU_NONE);
    return ERR_NONE;
}

int docmd_aon(arg_struct *arg) {
    flags.f.alpha_mode = 1;
    mode_alpha_entry = 0;
    set_menu(MENULEVEL_ALPHA, MENU_ALPHA1);
    return ERR_NONE;
}

int docmd_arot(arg_struct *arg) {
    if (reg_x->type == TYPE_REAL) {
	double x;
	char buf[44];
	int i, j;
	if (reg_alpha_length == 0)
	    goto done;
	x = ((vartype_real *) reg_x)->x;
	if (core_settings.ip_hack) {
	    if (x < 0)
		x = -floor(5e-9 - x);
	    else
		x = floor(5e-9 + x);
	} else {
	    if (x < 0)
		x = -floor(-x);
	    else
		x = floor(x);
	}
	j = (int) fmod(x, reg_alpha_length);
	if (j == 0)
	    goto done;
	if (j < 0)
	    j += reg_alpha_length;
	for (i = 0; i < reg_alpha_length; i++)
	    buf[i] = reg_alpha[i];
	for (i = 0; i < reg_alpha_length; i++)
	    reg_alpha[i] = buf[(i + j) % reg_alpha_length];
	done:
	if (flags.f.trace_print && flags.f.printer_exists)
	    docmd_pra(NULL);
	return ERR_NONE;
    } else if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else
	return ERR_INVALID_TYPE;
}

int docmd_ashf(arg_struct *arg) {
    int i;
    reg_alpha_length -= 6;
    if (reg_alpha_length < 0)
	reg_alpha_length = 0;
    for (i = 0; i < reg_alpha_length; i++)
	reg_alpha[i] = reg_alpha[i + 6];
    if (flags.f.trace_print && flags.f.printer_exists)
	docmd_pra(NULL);
    return ERR_NONE;
}

static int mappable_asinh_r(double x, double *y) COMMANDS3_SECT;
static int mappable_asinh_r(double x, double *y) {
    *y = asinh(x);
    return ERR_NONE;
}

static int mappable_asinh_c(double xre, double xim, double *yre, double *yim)
								COMMANDS3_SECT;
static int mappable_asinh_c(double xre, double xim, double *yre, double *yim) {
    return math_asinh_acosh(xre, xim, yre, yim, 1);
}

int docmd_asinh(arg_struct *arg) {
    if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else {
	vartype *v;
	int err = map_unary(reg_x, &v, mappable_asinh_r, mappable_asinh_c);
	if (err == ERR_NONE)
	    unary_result(v);
	return err;
    }
}

static int mappable_atanh_r(double x, double *y) COMMANDS3_SECT;
static int mappable_atanh_r(double x, double *y) {
    if (x == 1 || x == -1)
	return ERR_INVALID_DATA;
    *y = atanh(x);

    /* Theoretically, you could go out of range, but in practice,
     * you can't get close enough to the critical values to cause
     * trouble.
     */
    return ERR_NONE;
}

int docmd_atanh(arg_struct *arg) {
    vartype *v;
    int err;
    if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else if (reg_x->type == TYPE_REAL) {
	double x = ((vartype_real *) reg_x)->x;
	if (x == 1 || x == -1)
	    return ERR_INVALID_DATA;
	if (x < -1 || x > 1) {
	    if (flags.f.real_result_only)
		return ERR_INVALID_DATA;
	    else {
		double tre, tim;
		err = math_atanh(x, 0, &tre, &tim);
		if (err != ERR_NONE)
		    return err;
		v = new_complex(tre, tim);
	    }
	} else
	    v = new_real(atanh(x));
	if (v == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
	unary_result(v);
	return ERR_NONE;
    } else {
	err = map_unary(reg_x, &v, mappable_atanh_r, math_atanh);
	if (err == ERR_NONE)
	    unary_result(v);
	return err;
    }
}

int docmd_atox(arg_struct *arg) {
    vartype *v = new_real(reg_alpha_length == 0 ? 0 :
				    (unsigned char) reg_alpha[0]);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    if (reg_alpha_length > 0) {
	int i;
	reg_alpha_length--;
	for (i = 0; i < reg_alpha_length; i++)
	    reg_alpha[i] = reg_alpha[i + 1];
    }
    if (flags.f.trace_print && flags.f.printer_exists)
	docmd_pra(NULL);
    recall_result(v);
    return ERR_NONE;
}

int docmd_baseadd(arg_struct *arg) {
    int8 x, y, res;
    int err;
    vartype *v;
    if ((err = get_base_param(reg_x, &x)) != ERR_NONE)
	return err;
    if ((err = get_base_param(reg_y, &y)) != ERR_NONE)
	return err;
    res = x + y;
    if ((err = base_range_check(&res)) != ERR_NONE)
	return err;
    v = new_real((double) res);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    binary_result(v);
    return ERR_NONE;
}

int docmd_basesub(arg_struct *arg) {
    int8 x, y, res;
    int err;
    vartype *v;
    if ((err = get_base_param(reg_x, &x)) != ERR_NONE)
	return err;
    if ((err = get_base_param(reg_y, &y)) != ERR_NONE)
	return err;
    res = y - x;
    if ((err = base_range_check(&res)) != ERR_NONE)
	return err;
    v = new_real((double) res);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    binary_result(v);
    return ERR_NONE;
}

int docmd_basemul(arg_struct *arg) {
    int8 x, y;
    double res;
    int err;
    vartype *v;
    if ((err = get_base_param(reg_x, &x)) != ERR_NONE)
	return err;
    if ((err = get_base_param(reg_y, &y)) != ERR_NONE)
	return err;
    /* I compute the result in 'double' arithmetic, because doing it
     * in int8 arithmetic could cause me to overlook an out-of-range
     * condition (e.g. 2^32 * 2^32).
     */
    res = ((double) x) * ((double) y);
    if (res < -34359738368.0) {
	if (flags.f.range_error_ignore)
	    res = -34359738368.0;
	else
	    return ERR_OUT_OF_RANGE;
    } else if (res > 34359738367.0) {
	if (flags.f.range_error_ignore)
	    res = 34359738367.0;
	else
	    return ERR_OUT_OF_RANGE;
    }
    v = new_real(res);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    binary_result(v);
    return ERR_NONE;
}

int docmd_basediv(arg_struct *arg) {
    int8 x, y, res;
    int err;
    vartype *v;
    if ((err = get_base_param(reg_x, &x)) != ERR_NONE)
	return err;
    if ((err = get_base_param(reg_y, &y)) != ERR_NONE)
	return err;
    if (y == 0)
	return ERR_DIVIDE_BY_0;
    res = y / x;
    if ((err = base_range_check(&res)) != ERR_NONE)
	return err;
    v = new_real((double) res);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    binary_result(v);
    return ERR_NONE;
}

int docmd_basechs(arg_struct *arg) {
    int8 x;
    int err;
    if ((err = get_base_param(reg_x, &x)) != ERR_NONE)
	return err;
    if (x == LL(-34359738368)) {
	if (flags.f.range_error_ignore)
	    x = LL(34359738367);
	else
	    return ERR_OUT_OF_RANGE;
    } else
	x = -x;
    ((vartype_real *) reg_x)->x = (double) x;
    return ERR_NONE;
}

static struct {
    double x;
    double x2;
    double y;
    double y2;
    double xy;
    double n;
    double lnx;
    double lnx2;
    double lny;
    double lny2;
    double lnxlny;
    double xlny;
    double ylnx;
} sum;

static int get_summation() COMMANDS3_SECT;
static int get_summation() {
    /* Check if summation registers are OK */
    int4 first = mode_sigma_reg;
    int4 last = first + (flags.f.all_sigma ? 13 : 6);
    int4 size, i;
    vartype *regs = recall_var("REGS", 4);
    vartype_realmatrix *r;
    double_or_string *sigmaregs;
    if (regs == NULL)
	return ERR_SIZE_ERROR;
    if (regs->type != TYPE_REALMATRIX)
	return ERR_INVALID_TYPE;
    r = (vartype_realmatrix *) regs;
    size = r->rows * r->columns;
    if (last > size)
	return ERR_SIZE_ERROR;
    for (i = first; i < last; i++)
	if (r->array->is_string[i])
	    return ERR_ALPHA_DATA_IS_INVALID;
    sigmaregs = r->array->data + first;
    sum.x = sigmaregs[0].d;
    sum.x2 = sigmaregs[1].d;
    sum.y = sigmaregs[2].d;
    sum.y2 = sigmaregs[3].d;
    sum.xy = sigmaregs[4].d;
    sum.n = sigmaregs[5].d;
    if (flags.f.all_sigma) {
	sum.lnx = sigmaregs[6].d;
	sum.lnx2 = sigmaregs[7].d;
	sum.lny = sigmaregs[8].d;
	sum.lny2 = sigmaregs[9].d;
	sum.lnxlny = sigmaregs[10].d;
	sum.xlny = sigmaregs[11].d;
	sum.ylnx = sigmaregs[12].d;
    }
    return ERR_NONE;
}
    
static struct {
    double x;
    double x2;
    double y;
    double y2;
    double xy;
    double n;
    int ln_before;
    int exp_after;
    int valid;
    double slope;
    double yint;
} model;

#define MODEL_NONE -1
#define MODEL_LIN 0
#define MODEL_LOG 1
#define MODEL_EXP 2
#define MODEL_PWR 3

static int get_model_summation(int modl) COMMANDS3_SECT;
static int get_model_summation(int modl) {
    int err = get_summation();
    if (err != ERR_NONE)
	return err;
    switch (modl) {
	case MODEL_LIN:
	    model.xy = sum.xy;
	    model.ln_before = 0;
	    model.exp_after = 0;
	    break;
	case MODEL_LOG:
	    if (flags.f.log_fit_invalid)
		return ERR_INVALID_FORECAST_MODEL;
	    model.xy = sum.ylnx;
	    model.ln_before = 1;
	    model.exp_after = 0;
	    break;
	case MODEL_EXP:
	    if (flags.f.exp_fit_invalid)
		return ERR_INVALID_FORECAST_MODEL;
	    model.xy = sum.xlny;
	    model.ln_before = 0;
	    model.exp_after = 1;
	    break;
	case MODEL_PWR:
	    if (flags.f.pwr_fit_invalid)
		return ERR_INVALID_FORECAST_MODEL;
	    model.xy = sum.lnxlny;
	    model.ln_before = 1;
	    model.exp_after = 1;
	    break;
	default:
	    return ERR_INVALID_FORECAST_MODEL;
    }
    if (model.ln_before) {
	model.x = sum.lnx;
	model.x2 = sum.lnx2;
    } else {
	model.x = sum.x;
	model.x2 = sum.x2;
    }
    if (model.exp_after) {
	model.y = sum.lny;
	model.y2 = sum.lny2;
    } else {
	model.y = sum.y;
	model.y2 = sum.y2;
    }
    model.n = sum.n;
    return ERR_NONE;
}

static int corr_helper(int modl, double *r) COMMANDS3_SECT;
static int corr_helper(int modl, double *r) {
    double cov, varx, vary, v, tr;
    int err = get_model_summation(modl);
    if (err != ERR_NONE)
	return err;
    if (model.n == 0 || model.n == 1)
	return ERR_STAT_MATH_ERROR;
    cov = model.xy - model.x * model.y / model.n;
    varx = model.x2 - model.x * model.x / model.n;
    vary = model.y2 - model.y * model.y / model.n;
    if (varx <= 0 || vary <= 0)
	return ERR_STAT_MATH_ERROR;
    v = varx * vary;
    if (v == 0)
	return ERR_STAT_MATH_ERROR;
    tr = cov / sqrt(v);
    if (tr < -1)
	tr = -1;
    else if (tr > 1)
	tr = 1;
    *r = tr;
    return ERR_NONE;
}

static int slope_yint_helper() COMMANDS3_SECT;
static int slope_yint_helper() {
    /* The caller should have made sure that 'model' is up to date
     * by calling get_model_summation() first.
     */
    double cov, varx, meanx, meany;
    int inf;
    if (model.n == 0 || model.n == 1)
	return ERR_STAT_MATH_ERROR;
    cov = model.xy - model.x * model.y / model.n;
    varx = model.x2 - model.x * model.x / model.n;
    if (varx == 0)
	return ERR_STAT_MATH_ERROR;
    model.slope = cov / varx;
    if ((inf = isinf(model.slope)) != 0)
	model.slope = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
    meanx = model.x / model.n;
    meany = model.y / model.n;
    model.yint = meany - model.slope * meanx;
    if ((inf = isinf(model.yint)) != 0)
	model.yint = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
    return ERR_NONE;
}

static int get_model() COMMANDS3_SECT;
static int get_model() {
    if (flags.f.lin_fit)
	return MODEL_LIN;
    else if (flags.f.log_fit)
	return MODEL_LOG;
    else if (flags.f.exp_fit)
	return MODEL_EXP;
    else if (flags.f.pwr_fit)
	return MODEL_PWR;
    else
	return MODEL_NONE;
}

int docmd_best(arg_struct *arg) {
    int best = MODEL_NONE;
    double bestr = 0;
    int firsterr = ERR_NONE;
    int i;
    for (i = MODEL_LIN; i <= MODEL_PWR; i++) {
	double r;
	int err = corr_helper(i, &r);
	if (err == ERR_NONE) {
	    if (r < 0)
		r = -r;
	    if (r > bestr) {
		best = i;
		bestr = r;
	    }
	} else {
	    if (firsterr == ERR_NONE)
		firsterr = err;
	}
    }
    if (best == MODEL_NONE)
	best = MODEL_LIN;
    else
	firsterr = ERR_NONE;
    flags.f.lin_fit = best == MODEL_LIN;
    flags.f.log_fit = best == MODEL_LOG;
    flags.f.exp_fit = best == MODEL_EXP;
    flags.f.pwr_fit = best == MODEL_PWR;
    return firsterr;
}

static int base_helper(int base) COMMANDS3_SECT;
static int base_helper(int base) {
    if (program_running()) {
	int err = set_menu_return_err(MENULEVEL_APP, MENU_BASE);
	if (err != ERR_NONE)
	    return err;
	set_appmenu_exitcallback(2);
	baseapp = 1;
    }
    set_base(base);
    return ERR_NONE;
}

int docmd_binm(arg_struct *arg) {
    return base_helper(2);
}

int docmd_bit_t(arg_struct *arg) {
    int8 x, y;
    int err;
    if ((err = get_base_param(reg_x, &x)) != ERR_NONE)
	return err;
    if ((err = get_base_param(reg_y, &y)) != ERR_NONE)
	return err;
    if (x < 0 || x > 35)
	return ERR_INVALID_DATA;
    return (y & (1L << x)) != 0 ? ERR_YES : ERR_NO;
}

int docmd_corr(arg_struct *arg) {
    double r;
    int err;
    vartype *rv;
    err = corr_helper(get_model(), &r);
    if (err != ERR_NONE)
	return err;
    rv = new_real(r);
    if (rv == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    recall_result(rv);
    return ERR_NONE;
}

static int mappable_cosh_r(double x, double *y) COMMANDS3_SECT;
static int mappable_cosh_r(double x, double *y) {
    int inf;
    *y = cosh(x);
    if ((inf = isinf(*y)) != 0) {
	if (flags.f.range_error_ignore)
	    *y = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	else
	    return ERR_OUT_OF_RANGE;
    }
    return ERR_NONE;
}   

static int mappable_cosh_c(double xre, double xim,
	                             double *yre, double *yim) COMMANDS3_SECT;
static int mappable_cosh_c(double xre, double xim, double *yre, double *yim) {
    double sinhxre, coshxre;
    double sinxim, cosxim;
    int inf;
    sinhxre = sinh(xre);
    coshxre = cosh(xre);
    sincos(xim, &sinxim, &cosxim);
    *yre = coshxre * cosxim;
    if ((inf = isinf(*yre)) != 0) {
	if (flags.f.range_error_ignore)
	    *yre = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	else
	    return ERR_OUT_OF_RANGE;
    }
    *yim = sinhxre * sinxim;
    if ((inf = isinf(*yim)) != 0) {
	if (flags.f.range_error_ignore)
	    *yim = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	else
	    return ERR_OUT_OF_RANGE;
    }
    return ERR_NONE;
}

int docmd_cosh(arg_struct *arg) {
    if (reg_x->type != TYPE_STRING) {
	vartype *v;
	int err = map_unary(reg_x, &v, mappable_cosh_r, mappable_cosh_c);
	if (err == ERR_NONE)
	    unary_result(v);
	return err;
    } else
	return ERR_ALPHA_DATA_IS_INVALID;
}

/* NOTE: it is possible to generalize the cross product to more than
 * 3 dimensions... Something for Free42++ perhaps?
 */
int docmd_cross(arg_struct *arg) {
    if (reg_x->type == TYPE_STRING || reg_y->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else if (reg_x->type == TYPE_COMPLEX && reg_y->type == TYPE_COMPLEX) {
	vartype_complex *left = (vartype_complex *) reg_y;
	vartype_complex *right = (vartype_complex *) reg_x;
	vartype *v;
	double d = left->re * right->im - left->im * right->re;
	int inf;
	if ((inf = isinf(d)) != 0) {
	    if (flags.f.range_error_ignore)
		d = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	v = new_real(d);
	if (v == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
	binary_result(v);
	return ERR_NONE;
    } else if (reg_x->type == TYPE_REALMATRIX
			&& reg_y->type == TYPE_REALMATRIX) {
	vartype_realmatrix *left = (vartype_realmatrix *) reg_y;
	vartype_realmatrix *right = (vartype_realmatrix *) reg_x;
	int4 ls = left->rows * left->columns;
	int4 rs = right->rows * right->columns;
	int4 i;
	int inf;
	double xl, yl = 0, zl = 0, xr, yr = 0, zr = 0;
	double xres, yres, zres;
	vartype_realmatrix *res;
	if (ls > 3 || rs > 3)
	    return ERR_DIMENSION_ERROR;
	for (i = 0; i < ls; i++)
	    if (left->array->is_string[i])
		return ERR_ALPHA_DATA_IS_INVALID;
	for (i = 0; i < rs; i++)
	    if (right->array->is_string[i])
		return ERR_ALPHA_DATA_IS_INVALID;
	switch (ls) {
	    case 3: zl = left->array->data[2].d;
	    case 2: yl = left->array->data[1].d;
	    case 1: xl = left->array->data[0].d;
	}
	switch (rs) {
	    case 3: zr = right->array->data[2].d;
	    case 2: yr = right->array->data[1].d;
	    case 1: xr = right->array->data[0].d;
	}
	xres = yl * zr - zl * yr;
	if ((inf = isinf(xres)) != 0) {
	    if (flags.f.range_error_ignore)
		xres = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	yres = zl * xr - xl * zr;
	if ((inf = isinf(yres)) != 0) {
	    if (flags.f.range_error_ignore)
		yres = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	zres = xl * yr - yl * xr;
	if ((inf = isinf(zres)) != 0) {
	    if (flags.f.range_error_ignore)
		zres = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	res = (vartype_realmatrix *) new_realmatrix(1, 3);
	if (res == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
	res->array->data[0].d = xres;
	res->array->data[1].d = yres;
	res->array->data[2].d = zres;
	binary_result((vartype *) res);
	return ERR_NONE;
    } else
	return ERR_INVALID_TYPE;
}

int docmd_custom(arg_struct *arg) {
    if (mode_plainmenu == MENU_CUSTOM1
	    || mode_plainmenu == MENU_CUSTOM2
	    || mode_plainmenu == MENU_CUSTOM3)
	set_menu(MENULEVEL_PLAIN, MENU_NONE);
    else
	set_menu(MENULEVEL_PLAIN, MENU_CUSTOM1);
    return ERR_NONE;
}

int docmd_decm(arg_struct *arg) {
    return base_helper(10);
}

int docmd_delr(arg_struct *arg) {
    vartype *m, *newx;
    vartype_realmatrix *rm;
    vartype_complexmatrix *cm;
    int4 rows, columns, i, j, n, newi;
    int err, refcount;
    int interactive;

    switch (matedit_mode) {
	case 0:
	    return ERR_NONEXISTENT;
	case 1:
	case 3:
	    m = recall_var(matedit_name, matedit_length);
	    break;
	case 2:
	    m = matedit_x;
	    break;
	default:
	    return ERR_INTERNAL_ERROR;
    }
    if (m == NULL)
	return ERR_NONEXISTENT;
    if (m->type != TYPE_REALMATRIX && m->type != TYPE_COMPLEXMATRIX)
	return ERR_INVALID_TYPE;

    if (m->type == TYPE_REALMATRIX) {
	rm = (vartype_realmatrix *) m;
	rows = rm->rows;
	columns = rm->columns;
	refcount = rm->array->refcount;
    } else {
	cm = (vartype_complexmatrix *) m;
	rows = cm->rows;
	columns = cm->columns;
	refcount = cm->array->refcount;
    }

    if (rows == 1)
	return ERR_DIMENSION_ERROR;

    if (matedit_i >= rows)
	matedit_i = rows - 1;
    if (matedit_j >= columns)
	matedit_j = columns - 1;
    n = (matedit_i + 1) * columns + matedit_j;

    if (matedit_i == rows - 1) {
	/* Deleting the bottom row; we need to move up one row, and
	 * the next IJ value we show in X is going to be from *two* rows
	 * higher: if you delete row 4 and it's not the bottom row, then
	 * we'll show the value that's now in row 5; if it is the bottom
	 * row, we move up one, and show the value that's now in row 3.
	 */
	newi = matedit_i - 1;
	n -= 2 * columns;
    } else
	newi = matedit_i;

    interactive = matedit_mode == 2 || matedit_mode == 3;
    if (interactive) {
	if (m->type == TYPE_REALMATRIX) {
	    if (rm->array->is_string[n])
		newx = new_string(rm->array->data[n].s.text,
				  rm->array->data[n].s.length);
	    else
		newx = new_real(rm->array->data[n].d);
	} else
	    newx = new_complex(cm->array->data[2 * n],
			       cm->array->data[2 * n + 1]);

	if (newx == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
    }

    if (refcount == 1) {
	/* We have this array to ourselves so we can modify it in place
	 * We shuffle the array elements around so that if the reallocation
	 * succeeds, everything is in the right place, but if it fails, we
	 * still have all the data so we can roll everything back. And best
	 * of all, no temporary memory allocations needed!
	 */
	if (m->type == TYPE_REALMATRIX) {
	    for (j = 0; j < columns; j++) {
		double_or_string tempd =
				rm->array->data[matedit_i * columns + j];
		char tempc = rm->array->is_string[matedit_i * columns + j];
		for (i = matedit_i; i < rows - 1; i++) {
		    rm->array->data[i * columns + j] =
				rm->array->data[(i + 1) * columns + j];
		    rm->array->is_string[i * columns + j] =
				rm->array->is_string[(i + 1) * columns + j];
		}
		rm->array->data[(rows - 1) * columns + j] = tempd;
		rm->array->is_string[(rows - 1) * columns + j] = tempc;
	    }
	    err = dimension_array_ref(m, rows - 1, columns);
	    if (err != ERR_NONE) {
		/* Dang! Now we have to rotate everything back to where
		 * it was before. */
		for (j = 0; j < columns; j++) {
		    double_or_string tempd =
				    rm->array->data[(rows - 1) * columns + j];
		    char tempc = rm->array->is_string[(rows - 1) * columns + j];
		    for (i = rows - 1; i > matedit_i; i--) {
			rm->array->data[i * columns + j] =
				    rm->array->data[(i - 1) * columns + j];
			rm->array->is_string[i * columns + j] =
				    rm->array->is_string[(i - 1) * columns + j];
		    }
		    rm->array->data[matedit_i * columns + j] = tempd;
		    rm->array->is_string[matedit_i * columns + j] = tempc;
		}
		if (interactive)
		    free_vartype(newx);
		return err;
	    }
	} else {
	    for (j = 0; j < 2 * columns; j++) {
		double tempd = cm->array->data[matedit_i * 2 * columns + j];
		for (i = matedit_i; i < rows - 1; i++)
		    cm->array->data[i * 2 * columns + j] =
				cm->array->data[(i + 1) * 2 * columns + j];
		cm->array->data[(rows - 1) * 2 * columns + j] = tempd;
	    }
	    err = dimension_array_ref(m, rows - 1, columns);
	    if (err != ERR_NONE) {
		/* Dang! Now we have to rotate everything back to where
		 * it was before. */
		for (j = 0; j < 2 * columns; j++) {
		    double tempd = cm->array->data[(rows - 1) * 2 * columns + j];
		    for (i = rows - 1; i > matedit_i; i--)
			cm->array->data[i * 2 * columns + j] =
				    cm->array->data[(i - 1) * 2 * columns + j];
		    cm->array->data[matedit_i * 2 * columns + j] = tempd;
		}
		if (interactive)
		    free_vartype(newx);
		return err;
	    }
	}
    } else {
	/* We're sharing this array. I don't use disentangle() because it
	 * does not deal with resizing. */
	int4 newsize = (rows - 1) * columns;
	if (m->type == TYPE_REALMATRIX) {
	    realmatrix_data *array = (realmatrix_data *)
				malloc(sizeof(realmatrix_data));
	    if (array == NULL) {
		if (interactive)
		    free_vartype(newx);
		return ERR_INSUFFICIENT_MEMORY;
	    }
	    array->data = (double_or_string *)
				malloc(newsize * sizeof(double_or_string));
	    if (array->data == NULL) {
		if (interactive)
		    free_vartype(newx);
		free(array);
		return ERR_INSUFFICIENT_MEMORY;
	    }
	    array->is_string = (char *) malloc(newsize);
	    if (array->is_string == NULL) {
		if (interactive)
		    free_vartype(newx);
		free(array->data);
		free(array);
		return ERR_INSUFFICIENT_MEMORY;
	    }
	    for (i = 0; i < matedit_i * columns; i++) {
		array->is_string[i] = rm->array->is_string[i];
		array->data[i] = rm->array->data[i];
	    }
	    for (i = matedit_i * columns; i < newsize; i++) {
		array->is_string[i] = rm->array->is_string[i + columns];
		array->data[i] = rm->array->data[i + columns];
	    }
	    array->refcount = 1;
	    rm->array->refcount--;
	    rm->array = array;
	    rm->rows--;
	} else {
	    complexmatrix_data *array = (complexmatrix_data *)
				malloc(sizeof(complexmatrix_data));
	    if (array == NULL) {
		if (interactive)
		    free_vartype(newx);
		return ERR_INSUFFICIENT_MEMORY;
	    }
	    array->data = (double *) malloc(2 * newsize * sizeof(double));
	    if (array->data == NULL) {
		if (interactive)
		    free_vartype(newx);
		free(array);
		return ERR_INSUFFICIENT_MEMORY;
	    }
	    for (i = 0; i < 2 * matedit_i * columns; i++)
		array->data[i] = cm->array->data[i];
	    for (i = 2 * matedit_i * columns; i < 2 * newsize; i++)
		array->data[i] = cm->array->data[i + 2 * columns];
	    array->refcount = 1;
	    cm->array->refcount--;
	    cm->array = array;
	    cm->rows--;
	}
    }
    if (interactive) {
	free_vartype(reg_x);
	reg_x = newx;
    }
    matedit_i = newi;
    return ERR_NONE;
}

static void det_completion(int error, vartype *det) COMMANDS3_SECT;
static void det_completion(int error, vartype *det) {
    if (error == ERR_NONE)
	unary_result(det);
}

int docmd_det(arg_struct *arg) {
    if (reg_x->type == TYPE_REALMATRIX || reg_x->type == TYPE_COMPLEXMATRIX)
	return linalg_det(reg_x, det_completion);
    else if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else
	return ERR_INVALID_TYPE;
}

int docmd_dim(arg_struct *arg) {
    double x, y;
    int err;
    if (arg->type == ARGTYPE_IND_NUM
	    || arg->type == ARGTYPE_IND_STK
	    || arg->type == ARGTYPE_IND_STR) {
	err = resolve_ind_arg(arg);
	if (err != ERR_NONE)
	    return err;
    }
    if (arg->type != ARGTYPE_STR)
	return ERR_INVALID_TYPE;

    if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    if (reg_x->type != TYPE_REAL)
	return ERR_INVALID_TYPE;
    if (reg_y->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    if (reg_y->type != TYPE_REAL)
	return ERR_INVALID_TYPE;
    x = ((vartype_real *) reg_x)->x;
    if (x == 0)
	return ERR_DIMENSION_ERROR;
    if (x < 0)
	x = -x;
    if (x >= 2147483648.0)
	return ERR_INSUFFICIENT_MEMORY;
    y = ((vartype_real *) reg_y)->x;
    if (y == 0)
	return ERR_DIMENSION_ERROR;
    if (y < 0)
	y = -y;
    if (y >= 2147483648.0)
	return ERR_INSUFFICIENT_MEMORY;
    if (core_settings.ip_hack) {
	x += 5e-9;
	y += 5e-9;
    }
    return dimension_array(arg->val.text, arg->length, (int4) y, (int4) x);
}

int docmd_dot(arg_struct *arg) {
    /* TODO: look for range errors in intermediate results.
     * Right now, 1e300+1e300i DOT 1e300-1e300i returns NaN
     * on the Palm, because two infinities of opposite signs
     * are added. (Why no NaN on the PC? Weird stuff: doing
     * "d = xre * yre + xim * yim" yields a different result
     * than "d = xre * yre ; d += xim * yim". Go figure.)
     */
    vartype *v;
    if (reg_x->type == TYPE_STRING || reg_y->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else if (reg_x->type == TYPE_REALMATRIX && reg_y->type == TYPE_REALMATRIX) {
	vartype_realmatrix *rm1 = (vartype_realmatrix *) reg_x;
	vartype_realmatrix *rm2 = (vartype_realmatrix *) reg_y;
	int4 size = rm1->rows * rm1->columns;
	int4 i;
	double dot = 0;
	int inf;
	if (size != rm2->rows * rm2->columns)
	    return ERR_DIMENSION_ERROR;
	for (i = 0; i < size; i++)
	    if (rm1->array->is_string[i] || rm2->array->is_string[i])
		return ERR_ALPHA_DATA_IS_INVALID;
	for (i = 0; i < size; i++)
	    dot += rm1->array->data[i].d * rm2->array->data[i].d;
	if ((inf = isinf(dot)) != 0) {
	    if (flags.f.range_error_ignore)
		dot = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	v = new_real(dot);
    } else if ((reg_x->type == TYPE_REALMATRIX
		    && reg_y->type == TYPE_COMPLEXMATRIX)
		||
	       (reg_x->type == TYPE_COMPLEXMATRIX
		    && reg_y->type == TYPE_REALMATRIX)) {
	vartype_realmatrix *rm;
	vartype_complexmatrix *cm;
	int4 size, i;
	double dot_re = 0, dot_im = 0;
	int inf;
	if (reg_x->type == TYPE_REALMATRIX) {
	    rm = (vartype_realmatrix *) reg_x;
	    cm = (vartype_complexmatrix *) reg_y;
	} else {
	    rm = (vartype_realmatrix *) reg_y;
	    cm = (vartype_complexmatrix *) reg_x;
	}
	size = rm->rows * rm->columns;
	if (size != cm->rows * cm->columns)
	    return ERR_DIMENSION_ERROR;
	for (i = 0; i < size; i++)
	    if (rm->array->is_string[i])
		return ERR_ALPHA_DATA_IS_INVALID;
	for (i = 0; i < size; i++) {
	    dot_re += rm->array->data[i].d * cm->array->data[2 * i];
	    dot_im += rm->array->data[i].d * cm->array->data[2 * i + 1];
	}
	if ((inf = isinf(dot_re)) != 0) {
	    if (flags.f.range_error_ignore)
		dot_re = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	if ((inf = isinf(dot_im)) != 0) {
	    if (flags.f.range_error_ignore)
		dot_im = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	v = new_complex(dot_re, dot_im);
    } else if (reg_x->type == TYPE_COMPLEXMATRIX
		    && reg_y->type == TYPE_COMPLEXMATRIX) {
	vartype_complexmatrix *cm1 = (vartype_complexmatrix *) reg_x;
	vartype_complexmatrix *cm2 = (vartype_complexmatrix *) reg_y;
	int4 size, i;
	double dot_re = 0, dot_im = 0;
	int inf;
	size = cm1->rows * cm1->columns;
	if (size != cm2->rows * cm2->columns)
	    return ERR_DIMENSION_ERROR;
	size *= 2;
	for (i = 0; i < size; i += 2) {
	    double re1 = cm1->array->data[i];
	    double im1 = cm1->array->data[i + 1];
	    double re2 = cm2->array->data[i];
	    double im2 = cm2->array->data[i + 1];
	    dot_re += re1 * re2 - im1 * im2;
	    dot_im += re1 * im2 + re2 * im1;
	}
	if ((inf = isinf(dot_re)) != 0) {
	    if (flags.f.range_error_ignore)
		dot_re = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	if ((inf = isinf(dot_im)) != 0) {
	    if (flags.f.range_error_ignore)
		dot_im = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	v = new_complex(dot_re, dot_im);
    } else if (reg_x->type == TYPE_COMPLEX && reg_y->type == TYPE_COMPLEX) {
	vartype_complex *x = (vartype_complex *) reg_x;
	vartype_complex *y = (vartype_complex *) reg_y;
	double d = x->re * y->re + x->im * y->im;
	int inf;
	if ((inf = isinf(d)) != 0) {
	    if (flags.f.range_error_ignore)
		d = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	}
	v = new_real(d);
    } else
	return ERR_INVALID_TYPE;
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    binary_result(v);
    return ERR_NONE;
}

int matedit_get_dim(int4 *rows, int4 *columns) {
    vartype *m;

    switch (matedit_mode) {
	case 0:
	    return ERR_NONEXISTENT;
	case 1:
	case 3:
	    m = recall_var(matedit_name, matedit_length);
	    break;
	case 2:
	    m = matedit_x;
	    break;
	default:
	    return ERR_INTERNAL_ERROR;
    }

    if (m == NULL)
	return ERR_NONEXISTENT;

    if (m->type == TYPE_REALMATRIX) {
	vartype_realmatrix *rm = (vartype_realmatrix *) m;
	*rows = rm->rows;
	*columns = rm->columns;
	return ERR_NONE;
    } else if (m->type == TYPE_COMPLEXMATRIX) {
	vartype_complexmatrix *cm = (vartype_complexmatrix *) m;
	*rows = cm->rows;
	*columns = cm->columns;
	return ERR_NONE;
    } else
	return ERR_INVALID_TYPE;
}

/* NOTE: this is a callback for set_menu(); it is declared in
 * core_display.h but defined here.
 */
int appmenu_exitcallback_1(int menuid) {
    if (menuid == MENU_MATRIX_EDIT1 || menuid == MENU_MATRIX_EDIT2) {
	/* Just switching menus within the editor */
	mode_appmenu = menuid;
	set_appmenu_exitcallback(1);
	return ERR_NONE;
    } else {
	/* The user is trying to leave the editor; we only allow that
	 * if storing X is successful. */
	int err = docmd_stoel(NULL);
	if (err == ERR_INSUFFICIENT_MEMORY)
	    /* There's no graceful way to handle this, at least not without
	     * some code restructuring (TODO); this is the one error that
	     * should *not* prevent the user from getting out of the matrix
	     * editor! Unfortunately, setting the error to ERR_NONE means we
	     * aren't reporting the fact that changing the matrix element
	     * was unsuccessful.
	     */
	    err = ERR_NONE;
	if (err != ERR_NONE) {
	    /* Reinstate the callback so we'll get called again when
	     * the user tries to leave again. */
	    set_appmenu_exitcallback(1);
	    return err;
	}
	if (matedit_mode == 2) {
	    free_vartype(reg_x);
	    reg_x = matedit_x;
	    matedit_x = NULL;
	}
	matedit_mode = 0;
	flags.f.grow = 0;
	flags.f.stack_lift_disable = 0;
	/* Note: no need to check the value returned by set_menu() here:
	 * since we're in a set_menu() callback, we know that there is no
	 * callback registered at the moment (set_menu() unregisters
	 * callbacks just before invoking them), so these set_menu() calls
	 * won't fail.
	 */
	if (menuid == MENU_NONE)
	    set_menu(MENULEVEL_APP, matedit_prev_appmenu);
	else
	    set_menu(MENULEVEL_APP, menuid);
	return ERR_NONE;
    }
}

static int finish_edit() COMMANDS3_SECT;
static int finish_edit() {
    if (matedit_mode == 2 || matedit_mode == 3)
	/* Try to finish current interactive editing session using
	 * set_menu(). The callback (appmenu_exitcallback) will
	 * return its error status to set_menu(), which passes it
	 * on to us.
	 */
	return set_menu_return_err(MENULEVEL_APP, MENU_NONE);
    else
	return ERR_NONE;
}

int docmd_edit(arg_struct *arg) {
    int err = finish_edit();
    if (err != ERR_NONE)
	return err;
    if (reg_x->type == TYPE_REALMATRIX || reg_x->type == TYPE_COMPLEXMATRIX) {
	vartype *v;
	if (reg_x->type == TYPE_REALMATRIX) {
	    vartype_realmatrix *rm = (vartype_realmatrix *) reg_x;
	    if (rm->array->is_string[0])
		v = new_string(rm->array->data[0].s.text,
			       rm->array->data[0].s.length);
	    else
		v = new_real(rm->array->data[0].d);
	} else {
	    vartype_complexmatrix *cm = (vartype_complexmatrix *) reg_x;
	    v = new_complex(cm->array->data[0], cm->array->data[1]);
	}
	if (v == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
	matedit_mode = 2;
	flags.f.grow = 0;
	matedit_x = reg_x;
	reg_x = v;
	matedit_i = 0;
	matedit_j = 0;
	if (mode_appmenu >= MENU_MATRIX1 && mode_appmenu <= MENU_MATRIX_SIMQ)
	    matedit_prev_appmenu = mode_appmenu;
	else
	    matedit_prev_appmenu = MENU_NONE;
	set_menu(MENULEVEL_APP, MENU_MATRIX_EDIT1);
	set_appmenu_exitcallback(1);
	return ERR_NONE;
    } else
	return ERR_INVALID_TYPE;
}

int docmd_editn(arg_struct *arg) {
    vartype *m;
    int err;

    err = finish_edit();
    if (err != ERR_NONE)
	return err;

    if (arg->type == ARGTYPE_IND_NUM
	    || arg->type == ARGTYPE_IND_STK
	    || arg->type == ARGTYPE_IND_STR) {
	err = resolve_ind_arg(arg);
	if (err != ERR_NONE)
	    return err;
    }
    if (arg->type != ARGTYPE_STR)
	return ERR_INVALID_TYPE;
    m = recall_var(arg->val.text, arg->length);
    if (m == NULL)
	return ERR_NONEXISTENT;
    else if (m->type != TYPE_REALMATRIX && m->type != TYPE_COMPLEXMATRIX)
	return ERR_INVALID_TYPE;
    else {
	vartype *v;
	int i;
	if (m->type == TYPE_REALMATRIX) {
	    vartype_realmatrix *rm = (vartype_realmatrix *) m;
	    if (rm->array->is_string[0])
		v = new_string(rm->array->data[0].s.text,
			       rm->array->data[0].s.length);
	    else
		v = new_real(rm->array->data[0].d);
	} else {
	    vartype_complexmatrix *cm = (vartype_complexmatrix *) m;
	    v = new_complex(cm->array->data[0], cm->array->data[1]);
	}
	if (v == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
	/* TODO: implement a mechanism that locks a matrix while it is
	 * under edit. While locked, operations such as CLV, DIM, or
	 * assignment should fail with ERR_RESTRICTED_OPERATION.
	 */
	matedit_mode = 3;
	flags.f.grow = 0;
	matedit_length = arg->length;
	for (i = 0; i < matedit_length; i++)
	    matedit_name[i] = arg->val.text[i];
	free_vartype(reg_x);
	reg_x = v;
	matedit_i = 0;
	matedit_j = 0;
	if (mode_appmenu >= MENU_MATRIX1 && mode_appmenu <= MENU_MATRIX_SIMQ)
	    matedit_prev_appmenu = mode_appmenu;
	else
	    matedit_prev_appmenu = MENU_NONE;
	set_menu(MENULEVEL_APP, MENU_MATRIX_EDIT1);
	set_appmenu_exitcallback(1);
	return ERR_NONE;
    }
}

int docmd_exitall(arg_struct *arg) {
    return set_menu_return_err(MENULEVEL_APP, MENU_NONE);
}

int docmd_expf(arg_struct *arg) {
    flags.f.lin_fit = 0;
    flags.f.log_fit = 0;
    flags.f.exp_fit = 1;
    flags.f.pwr_fit = 0;
    return ERR_NONE;
}

static int mappable_e_pow_x_1(double x, double *y) COMMANDS3_SECT;
static int mappable_e_pow_x_1(double x, double *y) {
    *y = expm1(x);
    if (isinf(*y) != 0) {
	if (flags.f.range_error_ignore)
	    *y = POS_HUGE_DOUBLE;
	else
	    return ERR_OUT_OF_RANGE;
    }
    return ERR_NONE;
}   

int docmd_e_pow_x_1(arg_struct *arg) {
    if (reg_x->type == TYPE_REAL || reg_x->type == TYPE_REALMATRIX) {
	vartype *v;
	int err = map_unary(reg_x, &v, mappable_e_pow_x_1, NULL);
	if (err == ERR_NONE)
	    unary_result(v);
	return err;
    } else if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else
	return ERR_INVALID_TYPE;
}

static int mappable_fcstx(double x, double *y) COMMANDS3_SECT;
static int mappable_fcstx(double x, double *y) {
    int inf;
    if (model.exp_after) {
	if (x <= 0)
	    return ERR_INVALID_FORECAST_MODEL;
	x = log(x);
    }
    if (model.slope == 0)
	return ERR_STAT_MATH_ERROR;
    x = (x - model.yint) / model.slope;
    if (model.ln_before)
	x = exp(x);
    if ((inf = isinf(x)) != 0)
	x = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
    *y = x;
    return ERR_NONE;
}

int docmd_fcstx(arg_struct *arg) {
    int err = get_model_summation(get_model());
    vartype *v;
    if (err != ERR_NONE)
	return err;
    err = slope_yint_helper();
    if (err != ERR_NONE)
	return err;
    if (reg_x->type == TYPE_REAL || reg_x->type == TYPE_REALMATRIX) {
	err = map_unary(reg_x, &v, mappable_fcstx, NULL);
	if (err == ERR_NONE)
	    unary_result(v);
	return err;
    } else if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else
	return ERR_INVALID_TYPE;
}

static int mappable_fcsty(double x, double *y) COMMANDS3_SECT;
static int mappable_fcsty(double x, double *y) {
    int inf;
    if (model.ln_before) {
	if (x <= 0)
	    return ERR_INVALID_FORECAST_MODEL;
	x = log(x);
    }
    x = x * model.slope + model.yint;
    if (model.exp_after)
	x = exp(x);
    if ((inf = isinf(x)) != 0)
	x = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
    *y = x;
    return ERR_NONE;
}

int docmd_fcsty(arg_struct *arg) {
    int err = get_model_summation(get_model());
    vartype *v;
    if (err != ERR_NONE)
	return err;
    err = slope_yint_helper();
    if (err != ERR_NONE)
	return err;
    if (reg_x->type == TYPE_REAL || reg_x->type == TYPE_REALMATRIX) {
	err = map_unary(reg_x, &v, mappable_fcsty, NULL);
	if (err == ERR_NONE)
	    unary_result(v);
	return err;
    } else if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else
	return ERR_INVALID_TYPE;
}

static int fnrm(vartype *m, double *norm) COMMANDS3_SECT;
static int fnrm(vartype *m, double *norm) {
    if (m->type == TYPE_REALMATRIX) {
	vartype_realmatrix *rm = (vartype_realmatrix *) m;
	int4 size = rm->rows * rm->columns;
	int4 i;
	double nrm = 0;
	for (i = 0; i < size; i++)
	    if (rm->array->is_string[i])
		return ERR_ALPHA_DATA_IS_INVALID;
	for (i = 0; i < size; i++) {
	    /* TODO -- overflows in intermediaries */
	    double x = rm->array->data[i].d;
	    nrm += x * x;
	}
	if (isinf(nrm)) {
	    if (flags.f.range_error_ignore)
		nrm = POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	} else
	    nrm = sqrt(nrm);
	*norm = nrm;
	return ERR_NONE;
    } else if (m->type == TYPE_COMPLEXMATRIX) {
	vartype_complexmatrix *cm = (vartype_complexmatrix *) m;
	int4 size = 2 * cm->rows * cm->columns;
	int4 i;
	double nrm = 0;
	for (i = 0; i < size; i++) {
	    /* TODO -- overflows in intermediaries */
	    double x = cm->array->data[i];
	    nrm += x * x;
	}
	if (isinf(nrm)) {
	    if (flags.f.range_error_ignore)
		nrm = POS_HUGE_DOUBLE;
	    else
		return ERR_OUT_OF_RANGE;
	} else
	    nrm = sqrt(nrm);
	*norm = nrm;
	return ERR_NONE;
    } else if (m->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    else
	return ERR_INVALID_TYPE;
}

int docmd_fnrm(arg_struct *arg) {
    double norm;
    vartype *v;
    int err = fnrm(reg_x, &norm);
    if (err != ERR_NONE)
	return err;
    v = new_real(norm);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    unary_result(v);
    return ERR_NONE;
}

int docmd_getm(arg_struct *arg) {
    vartype *m;
    double xx, yy;
    int4 x, y;

    switch (matedit_mode) {
	case 0:
	    return ERR_NONEXISTENT;
	case 1:
	case 3:
	    m = recall_var(matedit_name, matedit_length);
	    break;
	case 2:
	    m = matedit_x;
	    break;
	default:
	    return ERR_INTERNAL_ERROR;
    }

    if (m == NULL)
	return ERR_NONEXISTENT;

    if (m->type != TYPE_REALMATRIX && m->type != TYPE_COMPLEXMATRIX)
	/* Should not happen, but could, as long as I don't implement
	 * matrix locking. */
	return ERR_INVALID_TYPE;
    
    if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    if (reg_x->type != TYPE_REAL)
	return ERR_INVALID_TYPE;
    if (reg_y->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    if (reg_y->type != TYPE_REAL)
	return ERR_INVALID_TYPE;

    xx = ((vartype_real *) reg_x)->x;
    if (xx < 0)
	xx = -xx;
    if (xx >= 2147483648.0)
	return ERR_DIMENSION_ERROR;
    x = (int4) xx;

    yy = ((vartype_real *) reg_y)->x;
    if (yy < 0)
	yy = -yy;
    if (yy >= 2147483648.0)
	return ERR_DIMENSION_ERROR;
    y = (int4) yy;

    if (m->type == TYPE_REALMATRIX) {
	vartype_realmatrix *src, *dst;
	int4 i, j;
	src = (vartype_realmatrix *) m;
	if (src->rows < matedit_i + y || src->columns < matedit_j + x)
	    return ERR_DIMENSION_ERROR;
	dst = (vartype_realmatrix *) new_realmatrix(y, x);
	if (dst == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
	for (i = 0; i < y; i++)
	    for (j = 0; j < x; j++) {
		int4 n1 = (i + matedit_i) * src->columns + j + matedit_j;
		int4 n2 = i * dst->columns + j;
		dst->array->is_string[n2] = src->array->is_string[n1];
		dst->array->data[n2] = src->array->data[n1];
	    }
	binary_result((vartype *) dst);
	return ERR_NONE;
    } else /* m->type == TYPE_COMPLEXMATRIX */ {
	vartype_complexmatrix *src, *dst;
	int4 i, j;
	src = (vartype_complexmatrix *) m;
	if (src->rows < matedit_i + y || src->columns < matedit_j + x)
	    return ERR_DIMENSION_ERROR;
	dst = (vartype_complexmatrix *) new_complexmatrix(y, x);
	if (dst == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
	for (i = 0; i < y; i++)
	    for (j = 0; j < x; j++) {
		int4 n1 = (i + matedit_i) * src->columns + j + matedit_j;
		int4 n2 = i * dst->columns + j;
		dst->array->data[n2 * 2] = src->array->data[n1 * 2];
		dst->array->data[n2 * 2 + 1] = src->array->data[n1 * 2 + 1];
	    }
	binary_result((vartype *) dst);
	return ERR_NONE;
    }
}

int docmd_grow(arg_struct *arg) {
    flags.f.grow = 1;
    return ERR_NONE;
}

int docmd_hexm(arg_struct *arg) {
    return base_helper(16);
}

static int hms_add_or_sub(int add) COMMANDS3_SECT;
static int hms_add_or_sub(int add) {
    double x, y, rx, ry, res;
    int8 ix, iy, ixhr, iyhr, ires, ireshr;
    int neg, inf;
    vartype *v;

    if (reg_x->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    if (reg_x->type != TYPE_REAL)
	return ERR_INVALID_TYPE;
    if (reg_y->type == TYPE_STRING)
	return ERR_ALPHA_DATA_IS_INVALID;
    if (reg_y->type != TYPE_REAL)
	return ERR_INVALID_TYPE;

    x = ((vartype_real *) reg_x)->x;
    y = ((vartype_real *) reg_y)->x;
    if (x < 0) {
	x = -x;
	add = !add;
    }
    if (y < 0) {
	y = -y;
	add = !add;
	neg = 1;
    } else
	neg = 0;

    /* I'm doing the computation in integer math. The nastiness is necessary,
     * because this is one of those cases (along with ->HR, ISG, and DSE)
     * where binary representation errors can mess things up pretty badly...
     * For example, you enter 0.45, thinking 45 minutes; the closest binary
     * representation is 0.449999999..., which could easily be mistaken for 44
     * minutes 99.99999... seconds -- 40 seconds off!
     */
    rx = floor(x);
    ry = floor(y);
    res = add ? ry + rx : ry - rx;
    ix = (int8) (((x - rx) * 1000000000000.0) + 0.5);
    iy = (int8) (((y - ry) * 1000000000000.0) + 0.5);
    ixhr = ix % LL(10000000000);
    iyhr = iy % LL(10000000000);
    ix /= LL(10000000000);
    iy /= LL(10000000000);
    ixhr += (ix % 100) * LL(6000000000);
    iyhr += (iy % 100) * LL(6000000000);
    ixhr += (ix / 100) * LL(360000000000);
    iyhr += (iy / 100) * LL(360000000000);
    ireshr = add ? iyhr + ixhr : iyhr - ixhr;
    while (ireshr < 0 && res > 0) {
	ireshr += LL(360000000000);
	res -= 1;
    }
    while (ireshr > 0 && res < 0) {
	ireshr -= LL(360000000000);
	res += 1;
    }
    ires = ireshr % LL(6000000000);
    ireshr /= LL(6000000000);
    ires += (ireshr % 60) * LL(10000000000);
    ires += (ireshr / 60) * LL(1000000000000);
    res += ires / 1000000000000.0;

    /* Round-off may have caused the minutes or seconds to reach 60;
     * detect this and fix...
     */
    res = fix_hms(res);

    if (neg)
	res = -res;

    if ((inf = isinf(res)) != 0) {
	if (flags.f.range_error_ignore)
	    res = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
	else
	    return ERR_OUT_OF_RANGE;
    }

    v = new_real(res);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    binary_result(v);
    return ERR_NONE;
}

int docmd_hmsadd(arg_struct *arg) {
    return hms_add_or_sub(1);
}

int docmd_hmssub(arg_struct *arg) {
    return hms_add_or_sub(0);
}

int docmd_i_add(arg_struct *arg) {
    int4 rows, columns;
    int err = matedit_get_dim(&rows, &columns);
    if (err != ERR_NONE)
	return err;
    if (++matedit_i >= rows) {
	flags.f.matrix_edge_wrap = 1;
	matedit_i = 0;
	if (++matedit_j >= columns) {
	    flags.f.matrix_end_wrap = 1;
	    matedit_j = 0;
	} else
	    flags.f.matrix_end_wrap = 0;
    } else {
	flags.f.matrix_edge_wrap = 0;
	flags.f.matrix_end_wrap = 0;
    }
    return ERR_NONE;
}

int docmd_i_sub(arg_struct *arg) {
    int4 rows, columns;
    int err = matedit_get_dim(&rows, &columns);
    if (err != ERR_NONE)
	return err;
    if (--matedit_i < 0) {
	flags.f.matrix_edge_wrap = 1;
	matedit_i = rows - 1;
	if (--matedit_j < 0) {
	    flags.f.matrix_end_wrap = 1;
	    matedit_j = columns - 1;
	} else
	    flags.f.matrix_end_wrap = 0;
    } else {
	flags.f.matrix_edge_wrap = 0;
	flags.f.matrix_end_wrap = 0;
    }
    return ERR_NONE;
}

void matedit_goto(int4 row, int4 column) {
    int4 rows, columns;
    int err = matedit_get_dim(&rows, &columns);
    if (err == ERR_NONE) {
	if (row == 0 || row > rows || column == 0 || column > columns)
	    err = ERR_DIMENSION_ERROR;
	else {
	    int prev_stack_lift_disable;
	    matedit_i = row - 1;
	    matedit_j = column - 1;
	    prev_stack_lift_disable = flags.f.stack_lift_disable;
	    flags.f.stack_lift_disable = 1;
	    err = docmd_rclel(NULL);
	    flags.f.stack_lift_disable = prev_stack_lift_disable;
	}
    }
    if (err != ERR_NONE) {
	display_error(err, 0);
	flush_display();
    }
}

int docmd_index(arg_struct *arg) {
    int err;
    vartype *m;
    int i;

    if (matedit_mode == 2 || matedit_mode == 3)
	/* TODO: on the real HP-42S, you get this error message the
	 * moment you say INDEX, that is, before even having to
	 * complete the command. Doing that in free42 would require
	 * a check in start_incomplete_command() or do_interactive().
	 * I'm putting that off until I have a better idea of whether
	 * this is an isolated special case (in which case I'll go
	 * for the quick-and-dirty hack) or something more general
	 * (in which case I'll redesign stuff if necessary).
	 */
	return ERR_RESTRICTED_OPERATION;

    if (arg->type == ARGTYPE_IND_NUM
	    || arg->type == ARGTYPE_IND_STK
	    || arg->type == ARGTYPE_IND_STR) {
	err = resolve_ind_arg(arg);
	if (err != ERR_NONE)
	    return err;
    }
    if (arg->type != ARGTYPE_STR)
	return ERR_INVALID_TYPE;

    m = recall_var(arg->val.text, arg->length);
    if (m == NULL)
	return ERR_NONEXISTENT;
    if (m->type != TYPE_REALMATRIX && m->type != TYPE_COMPLEXMATRIX)
	return ERR_INVALID_TYPE;

    /* TODO: keep a 'weak' lock on the matrix while it is indexed.
     * If it is deleted or redimensioned, I and J should be reset to 1.
     * Note that the current code uses a lazy, keep-it-safe approach
     * (it does not assume I and J are necessarily in bounds when I+, J-,
     * LEFT, RIGHT, etc., are called, but simply forces I and J within
     * bounds just before using them), but this does not always yield the
     * same result (try deleting the indexed matrix and then recreating it
     * immediately; on the real HP-42S, I and J are now both 1; on Free42,
     * I and J are unchanged).
     */
    matedit_mode = 1;
    matedit_i = 0;
    matedit_j = 0;
    matedit_length = arg->length;
    for (i = 0; i < matedit_length; i++)
	matedit_name[i] = arg->val.text[i];
    return ERR_NONE;
}

int docmd_octm(arg_struct *arg) {
    return base_helper(8);
}

int docmd_mean(arg_struct *arg) {
    double m;
    int inf;
    vartype *mx, *my;
    int err = get_summation();
    if (err != ERR_NONE)
	return err;
    if (sum.n == 0)
	return ERR_STAT_MATH_ERROR;
    m = sum.x / sum.n;
    if ((inf = isinf(m)) != 0)
	m = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
    mx = new_real(m);
    if (mx == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    m = sum.y / sum.n;
    if ((inf = isinf(m)) != 0)
	m = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
    my = new_real(m);
    if (my == NULL) {
	free_vartype(mx);
	return ERR_INSUFFICIENT_MEMORY;
    }
    free_vartype(reg_y);
    reg_y = my;
    free_vartype(reg_lastx);
    reg_lastx = reg_x;
    reg_x = mx;
    if (flags.f.trace_print && flags.f.printer_enable)
	docmd_prx(NULL);
    return ERR_NONE;
}

int docmd_sdev(arg_struct *arg) {
    int err = get_summation();
    double var;
    vartype *sx, *sy;
    if (err != ERR_NONE)
	return err;
    if (sum.n == 0 || sum.n == 1)
	return ERR_STAT_MATH_ERROR;
    var = (sum.x2 - (sum.x * sum.x / sum.n)) / (sum.n - 1);
    if (var < 0)
	return ERR_STAT_MATH_ERROR;
    if (isinf(var))
	sx = new_real(POS_HUGE_DOUBLE);
    else
	sx = new_real(sqrt(var));
    if (sx == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    var = (sum.y2 - (sum.y * sum.y / sum.n)) / (sum.n - 1);
    if (var < 0)
	return ERR_STAT_MATH_ERROR;
    if (isinf(var))
	sy = new_real(POS_HUGE_DOUBLE);
    else
	sy = new_real(sqrt(var));
    if (sy == NULL) {
	free_vartype(sx);
	return ERR_INSUFFICIENT_MEMORY;
    }
    free_vartype(reg_y);
    reg_y = sy;
    free_vartype(reg_lastx);
    reg_lastx = reg_x;
    reg_x = sx;
    if (flags.f.trace_print && flags.f.printer_enable)
	docmd_prx(NULL);
    return ERR_NONE;
}

int docmd_slope(arg_struct *arg) {
    int err = get_model_summation(get_model());
    vartype *v;
    if (err != ERR_NONE)
	return err;
    err = slope_yint_helper();
    if (err != ERR_NONE)
	return err;
    v = new_real(model.slope);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    recall_result(v);
    return ERR_NONE;
}

int docmd_sum(arg_struct *arg) {
    int err = get_summation();
    vartype *sx, *sy;
    if (err != ERR_NONE)
	return err;
    sx = new_real(sum.x);
    if (sx == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    sy = new_real(sum.y);
    if (sy == NULL) {
	free_vartype(sx);
	return ERR_INSUFFICIENT_MEMORY;
    }
    free_vartype(reg_lastx);
    free_vartype(reg_y);
    reg_y = sy;
    reg_lastx = reg_x;
    reg_x = sx;
    if (flags.f.trace_print && flags.f.printer_enable)
	docmd_prx(NULL);
    return ERR_NONE;
}

int docmd_uvec(arg_struct *arg) {
    double norm;
    int err;
    vartype *v;
    /* Check for complex matrices explicitly; everything else
     * is checked by fnrm(). NOTE: it's easy to make this function
     * handle complex matrices as well (fnrm() already does) but
     * since stage 1 of Free42 is all about HP-42S compatibility,
     * and the HP-42S doesn't allow complex matrices for UVEC, we
     * don't either.
     */
    if (reg_x->type == TYPE_COMPLEXMATRIX)
	return ERR_INVALID_TYPE;
    err = fnrm(reg_x, &norm);
    if (err != ERR_NONE)
	return err;
    if (norm == 0) {
	v = dup_vartype(reg_x);
	if (v == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
    } else {
	vartype_realmatrix *src = (vartype_realmatrix *) reg_x;
	vartype_realmatrix *dst;
	int4 rows = src->rows;
	int4 columns = src->columns;
	int4 size = rows * columns;
	int4 i;
	dst = (vartype_realmatrix *) new_realmatrix(rows, columns);
	if (dst == NULL)
	    return ERR_INSUFFICIENT_MEMORY;
	for (i = 0; i < size; i++)
	    dst->array->data[i].d = src->array->data[i].d / norm;
	v = (vartype *) dst;
    }
    unary_result(v);
    return ERR_NONE;
}

int docmd_wmean(arg_struct *arg) {
    double wm;
    int inf;
    vartype *v;
    int err = get_summation();
    if (err != ERR_NONE)
	return err;
    if (sum.y == 0)
	return ERR_STAT_MATH_ERROR;
    wm = sum.xy / sum.y;
    if ((inf = isinf(wm)) != 0)
	wm = inf < 0 ? NEG_HUGE_DOUBLE : POS_HUGE_DOUBLE;
    v = new_real(wm);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    recall_result(v);
    return ERR_NONE;
}

int docmd_yint(arg_struct *arg) {
    int err = get_model_summation(get_model());
    double yint;
    vartype *v;
    if (err != ERR_NONE)
	return err;
    err = slope_yint_helper();
    if (err != ERR_NONE)
	return err;
    if (model.exp_after) {
	yint = exp(model.yint);
	if (isinf(yint) != 0)
	    yint = POS_HUGE_DOUBLE;
    } else
	yint = model.yint;
    v = new_real(yint);
    if (v == NULL)
	return ERR_INSUFFICIENT_MEMORY;
    recall_result(v);
    return ERR_NONE;
}
