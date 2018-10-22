//
//  ViewController.swift
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

import UIKit

private enum NodeName : String {
    case input  = "nn_input"
    case output = "nn_output"
}

class ViewController: UIViewController {


    override func viewDidLoad() {
        super.viewDidLoad()
        
        recognize(image: #imageLiteral(resourceName: "digit"))
    }
    
    private func recognize(image: UIImage) {
        DispatchQueue.global().async {
            
            guard let modelPath = Bundle.main.path(forResource: "saved_model-encrypted", ofType: "pb") else {
                print("Model doesn't exist!")
                return
            }
            let inputNode:NodeName = .input
            let outputNode:NodeName = .output
            
            let predictor = MNISTPredictor.create(withPath:   modelPath,
                                                  inputNode:  inputNode,
                                                  outputNode: outputNode)
            predictor.loadModel(key: "BXKE0351PD9TXZ7XA8CK8XZU8XBGDM",
                                error: { error in
                print("Loading proto file is failed.\n\(error.localizedDescription)")
            })
            let inputSize = CGFloat(MNIST_IMAGE_PIXEL_SIDE_SIZE)
            let inputImage = image.resize(targetSize: inputSize)
            predictor.predict(image: inputImage,
                              success: { digit in
                print("Recognized Digit: \(digit)")
            }, error: {
                print("Recognition is failed!")
            })
            
        }
    }
}

extension MNISTPredictor {
    static func create<Name : RawRepresentable>(withPath path: String,
                                                inputNode:  Name,
                                                outputNode: Name) -> MNISTPredictor where Name.RawValue == String {
        return self.initWith(path,
                             inputNodeName: inputNode.rawValue,
                             outputNodeName: outputNode.rawValue)
    }
}
