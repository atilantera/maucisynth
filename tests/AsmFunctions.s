.section .data

.section .text
.globl nelio
.globl omaSini

nelio:
    # NOTE: this function stores its return value to register st0.

	push  %ebp        # Save caller function's ebp
	mov   %esp, %ebp  # Copy stack pointer (esp) to base pointer (ebp).
	                  # This must be done if we want to allocate space from
	                  # the stack for local variables.

	flds  0x8(%ebp)   # First parameter is in ebp + 8 bytes.
	                  # Load it into register st0.

	fmuls 0x8(%ebp)   # Multiply register st0 by first parameter.

                      # This

	pop   %ebp        # Restore caller function's ebp.
	ret               # Return (restore instruction address pointer eip from
	                  # the stack, adjust esp).

.section .data
sineCoeffA: .float 0f-0.1666667    # -1/6
sineCoeffB: .float 0f-0.05         # -1/20
.section .text
omaSini:
	# Parameters: float x  in 0x8(%ebp)
	# Return value in register %st(0).
	#
	# Register usage:
	# st0 = x
	# st1 = x^2
	# st2 = -1/6
	# st3 = 1/120
	# st4 = sineSum
	#
	# Calculates:
	# x - x^3/6 + x^5/120

	push  %ebp
	mov   %esp, %ebp

	# Save FPU state and registers to stack with fxsave instruction.
	# Fxsave requires 512 bytes, and the stack pointer given to it
	# must be divisible by 16. Use eax to calculate proper value for esp.
#	push  %eax
#	movl  $0xf, %eax
#	andl  %esp, %eax
#	addl  $512, %eax
#	subl  %eax, %esp
#	fxsave (%esp)

	# st0 = x
	# st1 = x
	# st2 = -1/6
	# st3 = -1/20.
	# st4 = x
	flds  0x8(%ebp)
	flds  sineCoeffB
	flds  sineCoeffA
	flds  0x8(%ebp)
	flds  0x8(%ebp)

    fmul  %st(1)         # st0 = x*x
    fxch  %st(1)         # st1 = x*x, st0 = x

	fmul  %st(1)         # st0 = x^3
	fmul  %st(2)         # st0 = -x^3/6

	fadd  %st(0), %st(4) # st4 = x - x^3/6

	fmul  %st(1)         # st0 = -x^3/6 * x^2   = -x^5/6
	fmul  %st(3)         # st0 = -x^5/6 * -1/20 = x^5/120
	fadd  %st(4), %st(0) # st0 = x - x^3/6 + x^5/120

#	fxrstor (%esp)
#	addl  %eax, %esp
#	pop   %eax
	pop   %ebp
	ret
