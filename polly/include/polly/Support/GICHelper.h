//===- Support/GICHelper.h -- Helper functions for ISL --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Helper functions for isl objects.
//
//===----------------------------------------------------------------------===//
//
#ifndef POLLY_SUPPORT_GIC_HELPER_H
#define POLLY_SUPPORT_GIC_HELPER_H

#include "llvm/ADT/APInt.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "isl/aff.h"
#include "isl/ctx.h"
#include "isl/isl-noexceptions.h"
#include "isl/map.h"
#include "isl/options.h"
#include "isl/set.h"
#include "isl/union_map.h"
#include "isl/union_set.h"
#include <functional>
#include <string>

struct isl_schedule;
struct isl_multi_aff;

namespace llvm {
class Value;
} // namespace llvm

namespace polly {

/// Translate an llvm::APInt to an isl_val.
///
/// Translate the bitsequence without sign information as provided by APInt into
/// a signed isl_val type. Depending on the value of @p IsSigned @p Int is
/// interpreted as unsigned value or as signed value in two's complement
/// representation.
///
/// Input IsSigned                 Output
///
///     0        0           ->    0
///     1        0           ->    1
///    00        0           ->    0
///    01        0           ->    1
///    10        0           ->    2
///    11        0           ->    3
///
///     0        1           ->    0
///     1        1           ->   -1
///    00        1           ->    0
///    01        1           ->    1
///    10        1           ->   -2
///    11        1           ->   -1
///
/// @param Ctx      The isl_ctx to create the isl_val in.
/// @param Int      The integer value to translate.
/// @param IsSigned If the APInt should be interpreted as signed or unsigned
///                 value.
///
/// @return The isl_val corresponding to @p Int.
__isl_give isl_val *isl_valFromAPInt(isl_ctx *Ctx, const llvm::APInt Int,
                                     bool IsSigned);

/// Translate an llvm::APInt to an isl::val.
///
/// Translate the bitsequence without sign information as provided by APInt into
/// a signed isl::val type. Depending on the value of @p IsSigned @p Int is
/// interpreted as unsigned value or as signed value in two's complement
/// representation.
///
/// Input IsSigned                 Output
///
///     0        0           ->    0
///     1        0           ->    1
///    00        0           ->    0
///    01        0           ->    1
///    10        0           ->    2
///    11        0           ->    3
///
///     0        1           ->    0
///     1        1           ->   -1
///    00        1           ->    0
///    01        1           ->    1
///    10        1           ->   -2
///    11        1           ->   -1
///
/// @param Ctx      The isl_ctx to create the isl::val in.
/// @param Int      The integer value to translate.
/// @param IsSigned If the APInt should be interpreted as signed or unsigned
///                 value.
///
/// @return The isl::val corresponding to @p Int.
inline isl::val valFromAPInt(isl_ctx *Ctx, const llvm::APInt Int,
                             bool IsSigned) {
  return isl::manage(isl_valFromAPInt(Ctx, Int, IsSigned));
}

/// Translate isl_val to llvm::APInt.
///
/// This function can only be called on isl_val values which are integers.
/// Calling this function with a non-integral rational, NaN or infinity value
/// is not allowed.
///
/// As the input isl_val may be negative, the APInt that this function returns
/// must always be interpreted as signed two's complement value. The bitwidth of
/// the generated APInt is always the minimal bitwidth necessary to model the
/// provided integer when interpreting the bit pattern as signed value.
///
/// Some example conversions are:
///
///   Input      Bits    Signed  Bitwidth
///       0 ->      0         0         1
///      -1 ->      1        -1         1
///       1 ->     01         1         2
///      -2 ->     10        -2         2
///       2 ->    010         2         3
///      -3 ->    101        -3         3
///       3 ->    011         3         3
///      -4 ->    100        -4         3
///       4 ->   0100         4         4
///
/// @param Val The isl val to translate.
///
/// @return The APInt value corresponding to @p Val.
llvm::APInt APIntFromVal(__isl_take isl_val *Val);

/// Translate isl::val to llvm::APInt.
///
/// This function can only be called on isl::val values which are integers.
/// Calling this function with a non-integral rational, NaN or infinity value
/// is not allowed.
///
/// As the input isl::val may be negative, the APInt that this function returns
/// must always be interpreted as signed two's complement value. The bitwidth of
/// the generated APInt is always the minimal bitwidth necessary to model the
/// provided integer when interpreting the bit pattern as signed value.
///
/// Some example conversions are:
///
///   Input      Bits    Signed  Bitwidth
///       0 ->      0         0         1
///      -1 ->      1        -1         1
///       1 ->     01         1         2
///      -2 ->     10        -2         2
///       2 ->    010         2         3
///      -3 ->    101        -3         3
///       3 ->    011         3         3
///      -4 ->    100        -4         3
///       4 ->   0100         4         4
///
/// @param Val The isl val to translate.
///
/// @return The APInt value corresponding to @p Val.
inline llvm::APInt APIntFromVal(isl::val V) {
  return APIntFromVal(V.release());
}

/// Get c++ string from Isl objects.
//@{
std::string stringFromIslObj(__isl_keep isl_map *map);
std::string stringFromIslObj(__isl_keep isl_union_map *umap);
std::string stringFromIslObj(__isl_keep isl_set *set);
std::string stringFromIslObj(__isl_keep isl_union_set *uset);
std::string stringFromIslObj(__isl_keep isl_schedule *schedule);
std::string stringFromIslObj(__isl_keep isl_multi_aff *maff);
std::string stringFromIslObj(__isl_keep isl_pw_multi_aff *pma);
std::string stringFromIslObj(__isl_keep isl_multi_pw_aff *mpa);
std::string stringFromIslObj(__isl_keep isl_union_pw_multi_aff *upma);
std::string stringFromIslObj(__isl_keep isl_aff *aff);
std::string stringFromIslObj(__isl_keep isl_pw_aff *pwaff);
std::string stringFromIslObj(__isl_keep isl_space *space);
//@}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_union_map *Map) {
  OS << polly::stringFromIslObj(Map);
  return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_map *Map) {
  OS << polly::stringFromIslObj(Map);
  return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_set *Set) {
  OS << polly::stringFromIslObj(Set);
  return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_pw_aff *Map) {
  OS << polly::stringFromIslObj(Map);
  return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_pw_multi_aff *PMA) {
  OS << polly::stringFromIslObj(PMA);
  return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_multi_aff *MA) {
  OS << polly::stringFromIslObj(MA);
  return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_union_pw_multi_aff *UPMA) {
  OS << polly::stringFromIslObj(UPMA);
  return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_schedule *Schedule) {
  OS << polly::stringFromIslObj(Schedule);
  return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                     __isl_keep isl_space *Space) {
  OS << polly::stringFromIslObj(Space);
  return OS;
}

/// Combine Prefix, Val (or Number) and Suffix to an isl-compatible name.
///
/// In case @p UseInstructionNames is set, this function returns:
///
/// @p Prefix + "_" + @p Val->getName() + @p Suffix
///
/// otherwise
///
/// @p Prefix + to_string(Number) + @p Suffix
///
/// We ignore the value names by default, as they may change between release
/// and debug mode and can consequently not be used when aiming for reproducible
/// builds. However, for debugging named statements are often helpful, hence
/// we allow their optional use.
std::string getIslCompatibleName(const std::string &Prefix,
                                 const llvm::Value *Val, long Number,
                                 const std::string &Suffix,
                                 bool UseInstructionNames);

/// Combine Prefix, Name (or Number) and Suffix to an isl-compatible name.
///
/// In case @p UseInstructionNames is set, this function returns:
///
/// @p Prefix + "_" + Name + @p Suffix
///
/// otherwise
///
/// @p Prefix + to_string(Number) + @p Suffix
///
/// We ignore @p Name by default, as they may change between release
/// and debug mode and can consequently not be used when aiming for reproducible
/// builds. However, for debugging named statements are often helpful, hence
/// we allow their optional use.
std::string getIslCompatibleName(const std::string &Prefix,
                                 const std::string &Middle, long Number,
                                 const std::string &Suffix,
                                 bool UseInstructionNames);

std::string getIslCompatibleName(const std::string &Prefix,
                                 const std::string &Middle,
                                 const std::string &Suffix);

inline llvm::DiagnosticInfoOptimizationBase &
operator<<(llvm::DiagnosticInfoOptimizationBase &OS,
           const isl::union_map &Obj) {
  OS << Obj.to_str();
  return OS;
}

/// Scope guard for code that allows arbitrary isl function to return an error
/// if the max-operations quota exceeds.
///
/// This allows to opt-in code sections that have known long executions times.
/// code not in a hot path can continue to assume that no unexpected error
/// occurs.
///
/// This is typically used inside a nested IslMaxOperationsGuard scope. The
/// IslMaxOperationsGuard defines the number of allowed base operations for some
/// code, IslQuotaScope defines where it is allowed to return an error result.
class IslQuotaScope {
  isl_ctx *IslCtx;
  int OldOnError;

public:
  IslQuotaScope() : IslCtx(nullptr) {}
  IslQuotaScope(const IslQuotaScope &) = delete;
  IslQuotaScope(IslQuotaScope &&Other)
      : IslCtx(Other.IslCtx), OldOnError(Other.OldOnError) {
    Other.IslCtx = nullptr;
  }
  const IslQuotaScope &operator=(IslQuotaScope &&Other) {
    std::swap(this->IslCtx, Other.IslCtx);
    std::swap(this->OldOnError, Other.OldOnError);
    return *this;
  }

  /// Enter a quota-aware scope.
  ///
  /// Should not be used directly. Use IslMaxOperationsGuard::enter() instead.
  explicit IslQuotaScope(isl_ctx *IslCtx, unsigned long LocalMaxOps)
      : IslCtx(IslCtx) {
    assert(IslCtx);
    assert(isl_ctx_get_max_operations(IslCtx) == 0 && "Incorrect nesting");
    if (LocalMaxOps == 0) {
      this->IslCtx = nullptr;
      return;
    }

    OldOnError = isl_options_get_on_error(IslCtx);
    isl_options_set_on_error(IslCtx, ISL_ON_ERROR_CONTINUE);
    isl_ctx_reset_error(IslCtx);
    isl_ctx_set_max_operations(IslCtx, LocalMaxOps);
  }

  ~IslQuotaScope() {
    if (!IslCtx)
      return;

    assert(isl_ctx_get_max_operations(IslCtx) > 0 && "Incorrect nesting");
    assert(isl_options_get_on_error(IslCtx) == ISL_ON_ERROR_CONTINUE &&
           "Incorrect nesting");
    isl_ctx_set_max_operations(IslCtx, 0);
    isl_options_set_on_error(IslCtx, OldOnError);
  }

  /// Return whether the current quota has exceeded.
  bool hasQuotaExceeded() const {
    if (!IslCtx)
      return false;

    return isl_ctx_last_error(IslCtx) == isl_error_quota;
  }
};

/// Scoped limit of ISL operations.
///
/// Limits the number of ISL operations during the lifetime of this object. The
/// idea is to use this as an RAII guard for the scope where the code is aware
/// that ISL can return errors even when all input is valid. After leaving the
/// scope, it will return to the error setting as it was before. That also means
/// that the error setting should not be changed while in that scope.
///
/// Such scopes are not allowed to be nested because the previous operations
/// counter cannot be reset to the previous state, or one that adds the
/// operations while being in the nested scope. Use therefore is only allowed
/// while currently a no operations-limit is active.
class IslMaxOperationsGuard {
private:
  /// The ISL context to set the operations limit.
  ///
  /// If set to nullptr, there is no need for any action at the end of the
  /// scope.
  isl_ctx *IslCtx;

  /// Maximum number of operations for the scope.
  unsigned long LocalMaxOps;

  /// When AutoEnter is enabled, holds the IslQuotaScope object.
  IslQuotaScope TopLevelScope;

public:
  /// Enter a max operations scope.
  ///
  /// @param IslCtx      The ISL context to set the operations limit for.
  /// @param LocalMaxOps Maximum number of operations allowed in the
  ///                    scope. If set to zero, no operations limit is enforced.
  /// @param AutoEnter   If true, automatically enters an IslQuotaScope such
  ///                    that isl operations may return quota errors
  ///                    immediately. If false, only starts the operations
  ///                    counter, but isl does not return quota errors before
  ///                    calling enter().
  IslMaxOperationsGuard(isl_ctx *IslCtx, unsigned long LocalMaxOps,
                        bool AutoEnter = true)
      : IslCtx(IslCtx), LocalMaxOps(LocalMaxOps) {
    assert(IslCtx);
    assert(isl_ctx_get_max_operations(IslCtx) == 0 &&
           "Nested max operations not supported");

    // Users of this guard may check whether the last error was isl_error_quota.
    // Reset the last error such that a previous out-of-quota error is not
    // mistaken to have occurred in the in this quota, even if the max number of
    // operations is set to infinite (LocalMaxOps == 0).
    isl_ctx_reset_error(IslCtx);

    if (LocalMaxOps == 0) {
      // No limit on operations; also disable restoring on_error/max_operations.
      this->IslCtx = nullptr;
      return;
    }

    isl_ctx_reset_operations(IslCtx);
    TopLevelScope = enter(AutoEnter);
  }

  /// Enter a scope that can handle out-of-quota errors.
  ///
  /// @param AllowReturnNull Whether the scoped code can handle out-of-quota
  ///                        errors. If false, returns a dummy scope object that
  ///                        does nothing.
  IslQuotaScope enter(bool AllowReturnNull = true) {
    return AllowReturnNull && IslCtx ? IslQuotaScope(IslCtx, LocalMaxOps)
                                     : IslQuotaScope();
  }

  /// Return whether the current quota has exceeded.
  bool hasQuotaExceeded() const {
    if (!IslCtx)
      return false;

    return isl_ctx_last_error(IslCtx) == isl_error_quota;
  }
};
} // end namespace polly

#endif
