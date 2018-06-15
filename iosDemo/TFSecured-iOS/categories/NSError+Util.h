//
//  NSError+Util.h
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSError(Util)

+ (instancetype)withMsg:(NSString*)msg code:(int)code localized:(NSString*)localized;

@end
