//
//  FDDebugAdditions.swift
//  FruitzOfDojo
//
//  Created by C.W. Betts on 1/16/17.
//  Copyright © 2017 C.W. Betts. All rights reserved.
//

import Foundation
import FruitzOfDojo
import FruitzOfDojo.FDDebug

public func FDLog(_ format: String, _ args: CVarArg...) {
	withVaList(args) { (argsR) -> Void in
		FDLogv(format, argsR)
	}
}

public func FDError(_ format: String, _ args: CVarArg...) {
	withVaList(args) { (argsR) -> Void in
		FDErrorv(format, argsR)
	}
}
