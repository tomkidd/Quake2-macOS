//
//  FDDisplayModeAdditions.swift
//  FruitzOfDojo
//
//  Created by C.W. Betts on 5/18/16.
//  Copyright © 2016 C.W. Betts. All rights reserved.
//

import Foundation
import FruitzOfDojo.FDDisplayMode

extension FDDisplayMode: Comparable {
    
}

public func ==(lhs: FDDisplayMode, rhs: FDDisplayMode) -> Bool {
    return lhs.isEqual(to: rhs)
}

public func <(lhs:FDDisplayMode, rhs: FDDisplayMode) -> Bool {
    return lhs.compare(rhs) == .orderedAscending
}

public func >(lhs:FDDisplayMode, rhs: FDDisplayMode) -> Bool {
    return lhs.compare(rhs) == .orderedDescending
}
