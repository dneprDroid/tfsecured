//
//  NSError+Util.m
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "NSError+Util.h"

@implementation NSError(Util)


+ (instancetype)withMsg:(NSString*)msg code:(int)code localized:(NSString*)localized {
    return [[NSError alloc]initWithDomain: msg
                                     code: code
                                 userInfo: localized ? @{NSLocalizedDescriptionKey : localized } : @{}];
}
@end
