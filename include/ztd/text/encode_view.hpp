// =============================================================================
//
// ztd.text
// Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================>

#pragma once

#ifndef ZTD_TEXT_ENCODE_VIEW_HPP
#define ZTD_TEXT_ENCODE_VIEW_HPP

#include <ztd/text/encode_iterator.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/encoding.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>

#include <ztd/text/subrange.hpp>
#include <ztd/text/detail/reconstruct.hpp>

#include <string_view>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_ranges Ranges, Views, and Iterators
	/// @{
	//////

	//////
	/// @brief A view over a range of code points, presenting the code points as code units. Uses the @p _Encoding
	/// specified to do so.
	///
	/// @tparam _Encoding The encoding to read the underlying range of code points as.
	/// @tparam _Range The range of input that will be fed into the _FromEncoding's decode operation.
	/// @tparam _ErrorHandler The error handler for any encode-step failures.
	/// @tparam _State The state type to use for the encode operations to intermediate code points.
	///
	/// @remarks The view presents code units one at a time, regardless of how many code units are output by one decode
	/// operation. This means if, for example, one (1) UTF-32 code point becomes four (4) UTF-8 code units, it will
	/// present each code unit one at a time. If you are looking to explicitly know what a single encode operation maps
	/// into as far as number of code points to code units (and vice-versa), you will have to use lower-level
	/// interfaces.
	//////
	template <typename _Encoding, typename _Range = ::std::basic_string_view<code_point_t<_Encoding>>,
		typename _ErrorHandler = __detail::__careless_handler, typename _State = encode_state_t<_Encoding>>
	class encode_view {
	private:
		using _StoredRange = __detail::__reconstruct_t<__detail::__remove_cvref_t<_Range>>;

	public:
		//////
		/// @brief The iterator type for this view.
		///
		//////
		using iterator = encode_iterator<_Encoding, _StoredRange, _ErrorHandler, _State>;
		//////
		/// @brief The sentinel type for this view.
		///
		//////
		using sentinel = encode_sentinel;
		//////
		/// @brief The underlying range type.
		///
		//////
		using range_type = _Range;
		//////
		/// @brief The encoding type used for transformations.
		///
		//////
		using encoding_type = _Encoding;
		//////
		/// @brief The error handler when an encode operation fails.
		///
		//////
		using error_handler_type = _ErrorHandler;
		//////
		/// @brief The state type used for encode operations.
		///
		//////
		using state_type = encode_state_t<encoding_type>;

		//////
		/// @brief Constructs an encode_view from the underlying range.
		///
		/// @param[in] __range The input range to wrap and iterate over.
		///
		/// @remarks the stored encoding, error handler, and state type are default-constructed.
		//////
		constexpr encode_view(range_type __range) noexcept : _M_it(::std::move(__range)) {
		}

		//////
		/// @brief Constructs an encode_view from the underlying range.
		///
		/// @param[in] __range The input range to wrap and iterate over.
		/// @param[in] __encoding The encoding object to call `.encode` or equivalent functionality on.
		//////
		constexpr encode_view(range_type __range, encoding_type __encoding) noexcept
		: _M_it(::std::move(__range), ::std::move(__encoding)) {
		}

		//////
		/// @brief Constructs an encode_view from the underlying range.
		///
		/// @param[in] __range The input range to wrap and iterate over.
		/// @param[in] __encoding The encoding object to call `.encode` or equivalent functionality on.
		/// @param[in] __error_handler A previously-made encode_view iterator.
		//////
		constexpr encode_view(
			range_type __range, encoding_type __encoding, error_handler_type __error_handler) noexcept
		: _M_it(::std::move(__range), ::std::move(__encoding), ::std::move(__error_handler)) {
		}

		//////
		/// @brief Constructs an encode_view from the underlying range.
		///
		/// @param[in] __range The input range to wrap and iterate over.
		/// @param[in] __encoding The encoding object to call `.encode` or equivalent functionality on.
		/// @param[in] __error_handler A previously-made encode_view iterator.
		/// @param[in] __state The state to user for the encode operation.
		//////
		constexpr encode_view(range_type __range, encoding_type __encoding, error_handler_type __error_handler,
			state_type __state) noexcept
		: _M_it(::std::move(__range), ::std::move(__encoding), ::std::move(__error_handler), ::std::move(__state)) {
		}

		//////
		/// @brief Constructs an encoding_view from one of its iterators, reconstituting the range.
		///
		/// @param[in] __it An previously-made encode_view iterator.
		//////
		constexpr encode_view(iterator __it) noexcept : _M_it(::std::move(__it)) {
		}

		//////
		/// @brief The beginning of the range. Uses a sentinel type and not a special iterator.
		//////
		constexpr iterator begin() const& noexcept {
			return this->_M_it;
		}

		//////
		/// @brief The beginning of the range. Uses a sentinel type and not a special iterator.
		//////
		constexpr iterator begin() && noexcept {
			return ::std::move(this->_M_it);
		}

		//////
		/// @brief The end of the range. Uses a sentinel type and not a special iterator.
		//////
		constexpr sentinel end() const noexcept {
			return sentinel();
		}

		//////
		/// @brief The reconstruct extension point for rebuilding an encoding view from its iterator and sentinel
		/// type.
		//////
		friend constexpr encode_view reconstruct(::std::in_place_type_t<encode_view>, iterator __it, sentinel) {
			return encode_view(::std::move(__it));
		}

	private:
		iterator _M_it;
	};

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_ENCODE_VIEW_HPP
