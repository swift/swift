/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

namespace Swift {
	using std::equal_to;

	namespace Detail {
		template<typename XIt, typename YIt, typename Length, typename Predicate>
		void computeLeastCommonSubsequenceMatrix(XIt xBegin, XIt xEnd, YIt yBegin, YIt yEnd, std::vector<Length>& result) {
			size_t width = std::distance(xBegin, xEnd) + 1;
			size_t height = std::distance(yBegin, yEnd)  + 1;
			result.resize(width * height);

			// Initialize first row & column
			for (size_t i = 0; i < width; ++i) {
				result[i] = 0;
			}
			for (size_t j = 0; j < height; ++j) {
				result[j*width] = 0;
			}

			// Compute the LCS lengths for subsets
			Predicate predicate;
			for (size_t i = 1; i < width; ++i) {
				for (size_t j = 1; j < height; ++j) {
					result[i + j*width] = (predicate(*(xBegin + i-1), *(yBegin + j-1)) ? result[(i-1) + (j-1)*width] + 1 : std::max(result[i + (j-1)*width], result[i-1 + (j*width)]));
				}
			}
		}
	}

	template<typename X, typename InsertRemovePredicate, typename UpdatePredicate>
	void computeIndexDiff(const std::vector<X>& x, const std::vector<X>& y, std::vector<size_t>& updates, std::vector<size_t>& postUpdates, std::vector<size_t>& removes, std::vector<size_t>& inserts) {
		InsertRemovePredicate insertRemovePredicate;
		UpdatePredicate updatePredicate;

		// Find & handle common prefix (Optimization to reduce LCS matrix size)
		typename std::vector<X>::const_iterator xBegin = x.begin();
		typename std::vector<X>::const_iterator yBegin = y.begin();
		while (xBegin < x.end() && yBegin < y.end() && insertRemovePredicate(*xBegin, *yBegin)) {
			if (updatePredicate(*xBegin, *yBegin)) {
				updates.push_back(std::distance(x.begin(), xBegin));
				postUpdates.push_back(std::distance(y.begin(), yBegin));
			}
			++xBegin;
			++yBegin;
		}
		size_t prefixLength = std::distance(x.begin(), xBegin);

		// Find & handle common suffix (Optimization to reduce LCS matrix size)
		typename std::vector<X>::const_reverse_iterator xEnd = x.rbegin();
		typename std::vector<X>::const_reverse_iterator yEnd = y.rbegin();
		while (xEnd.base() > xBegin && yEnd.base() > yBegin && insertRemovePredicate(*xEnd, *yEnd)) {
			if (updatePredicate(*xEnd, *yEnd)) {
				updates.push_back(std::distance(x.begin(), xEnd.base()) - 1);
				postUpdates.push_back(std::distance(y.begin(), yEnd.base()) - 1);
			}
			++xEnd;
			++yEnd;
		}

		// Compute lengths
		size_t xLength = std::distance(xBegin, xEnd.base());
		size_t yLength = std::distance(yBegin, yEnd.base());

		// Compute LCS matrix
		std::vector<unsigned int> lcs;
		Detail::computeLeastCommonSubsequenceMatrix<typename std::vector<X>::const_iterator, typename std::vector<X>::const_iterator, unsigned int, InsertRemovePredicate>(xBegin, xEnd.base(), yBegin, yEnd.base(), lcs);

		// Process LCS matrix
		size_t i = xLength;
		size_t j = yLength;
		const size_t width = xLength + 1;
		while (true) {
			if (i > 0 && j > 0 && insertRemovePredicate(x[prefixLength + i-1], y[prefixLength + j-1])) {
				// x[i-1] same
				if (updatePredicate(x[prefixLength + i - 1], y[prefixLength + j - 1])) {
					updates.push_back(prefixLength + i-1);
					postUpdates.push_back(prefixLength + j-1);
				}
				i -= 1;
				j -= 1;
			}
			else if (j > 0 && (i == 0 || lcs[i + (j-1)*width] >= lcs[i-1 + j*width])) {
				// y[j-1] added
				inserts.push_back(prefixLength + j-1);
				j -= 1;
			}
			else if (i > 0 && (j == 0 || lcs[i + (j-1)*width] < lcs[i-1 + j*width])) {
				// x[i-1] removed
				removes.push_back(prefixLength + i-1);
				i -= 1;
			}
			else {
				break;
			}
		}
	}
}
