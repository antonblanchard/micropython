/* Copyright 2016 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MAMBO_H__
#define __MAMBO_H__

static inline unsigned long callthru0(int command)
{
	register unsigned long c __asm("r3") = command;
	__asm volatile (".long 0x000eaeb0":"=r" (c):"r"(c));
	return c;
}

static inline unsigned long callthru2(int command, unsigned long arg1,
				      unsigned long arg2)
{
	register unsigned long c __asm("r3") = command;
	register unsigned long a1 __asm("r4") = arg1;
	register unsigned long a2 __asm("r5") = arg2;
	__asm volatile (".long 0x000eaeb0":"=r" (c):"r"(c), "r"(a1), "r"(a2));
	return c;
}

static inline unsigned long callthru3(int command, unsigned long arg1,
				      unsigned long arg2, unsigned long arg3)
{
	register unsigned long c __asm("r3") = command;
	register unsigned long a1 __asm("r4") = arg1;
	register unsigned long a2 __asm("r5") = arg2;
	register unsigned long a3 __asm("r6") = arg3;
	__asm volatile (".long 0x000eaeb0":"=r" (c):"r"(c), "r"(a1), "r"(a2),
		      "r"(a3));
	return c;
}

/* Mambo callthru commands */
#define SIM_WRITE_CONSOLE_CODE	0
#define SIM_EXIT_CODE		31
#define SIM_READ_CONSOLE_CODE	60
#define SIM_GET_TIME_CODE	70
#define SIM_CALL_TCL		86
#define SIM_BOGUS_DISK_READ	116
#define SIM_BOGUS_DISK_WRITE	117
#define SIM_BOGUS_DISK_INFO	118

#endif /* __MAMBO_H__ */
